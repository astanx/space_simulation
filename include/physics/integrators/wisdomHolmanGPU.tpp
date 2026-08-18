#pragma once

#include "debug/logger.h"

#include "physics/integrators/integratable.h"
#include "physics/integrators/wisdomHolmanGPUBuffers.h"

#include "physics/world/total.h"

#include "physics/systems/system.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/constants.h"

#include "render/modelSource.h"

#include "compute/context.h"
#include "compute/commandQueue.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "graphics/state/scopedBuffer.h"

#include <OpenCL/cl.h>
#include <iostream>

// Private functions
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::initKernels(WisdomHolmanGPUBuffers &gpu, Total total)
{
  int kernelTotal = static_cast<int>(total.object + total.orbital);

  this->halfKickKernel.setArg(0, gpu.positionsBuffer.get());
  this->halfKickKernel.setArg(1, gpu.musBuffer.get());
  this->halfKickKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->halfKickKernel.setArg(3, gpu.angularVelocitiesBuffer.get());
  this->halfKickKernel.setArg(4, gpu.quadrupoleTensorsBuffer.get());
  this->halfKickKernel.setArg(5, gpu.meanRadiiBuffer.get());
  this->halfKickKernel.setArg(6, gpu.centralBodyIndicesBuffer.get());
  this->halfKickKernel.setArg(7, gpu.loveIndicesBuffer.get());
  this->halfKickKernel.setArg(8, gpu.tidalFactorIndicesBuffer.get());
  this->halfKickKernel.setArg(9, gpu.loveNumbersBuffer.get());
  this->halfKickKernel.setArg(10, gpu.tidalFactorsBuffer.get());
  this->halfKickKernel.setArg(11, sizeof(int), &kernelTotal);

  this->halfKickLinearKernel.setArg(0, gpu.positionsBuffer.get());
  this->halfKickLinearKernel.setArg(1, gpu.musBuffer.get());
  this->halfKickLinearKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->halfKickLinearKernel.setArg(3, gpu.centralBodyIndicesBuffer.get());
  this->halfKickLinearKernel.setArg(4, sizeof(int), &kernelTotal);

  this->halfKickAngularKernel.setArg(0, gpu.positionsBuffer.get());
  this->halfKickAngularKernel.setArg(1, gpu.musBuffer.get());
  this->halfKickAngularKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->halfKickAngularKernel.setArg(3, gpu.angularVelocitiesBuffer.get());
  this->halfKickAngularKernel.setArg(4, gpu.quadrupoleTensorsBuffer.get());
  this->halfKickAngularKernel.setArg(5, gpu.inertiaTensorsBuffer.get());
  this->halfKickAngularKernel.setArg(6, gpu.meanRadiiBuffer.get());
  this->halfKickAngularKernel.setArg(7, gpu.loveIndicesBuffer.get());
  this->halfKickAngularKernel.setArg(8, gpu.tidalFactorIndicesBuffer.get());
  this->halfKickAngularKernel.setArg(9, gpu.loveNumbersBuffer.get());
  this->halfKickAngularKernel.setArg(10, gpu.tidalFactorsBuffer.get());
  this->halfKickAngularKernel.setArg(11, sizeof(int), &kernelTotal);

  this->driftOrbitalLinearKernel.setArg(0, gpu.positionsBuffer.get());
  this->driftOrbitalLinearKernel.setArg(1, gpu.musBuffer.get());
  this->driftOrbitalLinearKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->driftOrbitalLinearKernel.setArg(3, gpu.semiAxisesBuffer.get());
  this->driftOrbitalLinearKernel.setArg(4, gpu.eccentricitiesBuffer.get());
  this->driftOrbitalLinearKernel.setArg(5, gpu.inclinationsBuffer.get());
  this->driftOrbitalLinearKernel.setArg(6, gpu.longitudeBuffer.get());
  this->driftOrbitalLinearKernel.setArg(7, gpu.periapsisBuffer.get());
  this->driftOrbitalLinearKernel.setArg(8, gpu.meanAnomalyBuffer.get());
  this->driftOrbitalLinearKernel.setArg(9, gpu.meanMotionBuffer.get());
  this->driftOrbitalLinearKernel.setArg(10, gpu.centralBodyIndicesBuffer.get());

  this->driftObjectsLinearKernel.setArg(0, gpu.positionsBuffer.get());
  this->driftObjectsLinearKernel.setArg(1, gpu.velocitiesBuffer.get());

  this->driftAngularKernel.setArg(0, gpu.angularVelocitiesBuffer.get());
  this->driftAngularKernel.setArg(1, gpu.orientationsBuffer.get());
}
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::updateDt(Real dt)
{
  Real kickDt = dt * 0.5;
  this->halfKickKernel.setArg(12, sizeof(Real), &kickDt);
  this->halfKickLinearKernel.setArg(5, sizeof(Real), &kickDt);
  this->halfKickAngularKernel.setArg(12, sizeof(Real), &kickDt);
  this->driftOrbitalLinearKernel.setArg(11, sizeof(Real), &dt);
  this->driftObjectsLinearKernel.setArg(2, sizeof(Real), &dt);
  this->driftAngularKernel.setArg(2, sizeof(Real), &dt);
}

// Constructor
template <typename Real>
WisdomHolmanIntegratorGPU<Real>::WisdomHolmanIntegratorGPU(ResourceManager &resourceManager)
    : driftAngularKernel(resourceManager.GetKernel(Res::DRIFT_ANGULAR_KERNEL)),
      driftObjectsLinearKernel(resourceManager.GetKernel(Res::DRIFT_OBJECTS_LINEAR_KERNEL)),
      driftOrbitalLinearKernel(resourceManager.GetKernel(Res::DRIFT_ORBITAL_LINEAR_KERNEL)),
      halfKickLinearKernel(resourceManager.GetKernel(Res::HALF_KICK_LINEAR_KERNEL)),
      halfKickAngularKernel(resourceManager.GetKernel(Res::HALF_KICK_ANGULAR_KERNEL)),
      halfKickKernel(resourceManager.GetKernel(Res::HALF_KICK_KERNEL))
{
}

// Public functions
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::init(IntegratorGPUBuffers &gpu, Total &total, Context &ctx)
{
  WisdomHolmanGPUBuffers *whGpu = dynamic_cast<WisdomHolmanGPUBuffers *>(&gpu);
  if (!whGpu)
    Logger::logFatal("Wisdom Holman Integrator", "Wrong GPU data passed");
  this->initKernels(*whGpu, total);
}

template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::stepReal(CommandQueue &queue, Total &total, Real dt)
{
  this->updateDt(dt);

  // create one gl buffer here model matrxi parts
  // create cl buffer from it
  // fill it parallel with basic data

  // make buffer from vbo
  // make kernel / update driftAngular to write there
  // assign to each mesh range/index in vbo
  // use barnes-hut for objects mass < x in kick

  // Kick
  // this->queue.enqueueNDKernelBuffer(this->halfKickKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickLinearKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &total.total);

  // Drift
  queue.enqueueNDKernelBuffer(this->driftObjectsLinearKernel.get(), 1, &total.orbital, &total.object);
  queue.enqueueNDKernelBuffer(this->driftOrbitalLinearKernel.get(), 1, NULL, &total.orbital);
  queue.enqueueNDKernelBuffer(this->driftAngularKernel.get(), 1, NULL, &total.total);

  // Kick
  // this->queue.enqueueNDKernelBuffer(this->halfKickKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickLinearKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &total.total);
}