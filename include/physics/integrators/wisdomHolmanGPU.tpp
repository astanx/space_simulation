#pragma once

#include "debug/logger.h"

#include "physics/integrators/data/wisdomHolmanGPUBuffers.h"

#include "physics/world/total.h"

#include "compute/commandQueue.h"

#include "resources/manager/resourceManager.h"
#include "resources/resources.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
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

  this->halfKickObjectsLinearKernel.setArg(0, gpu.positionsBuffer.get());
  this->halfKickObjectsLinearKernel.setArg(1, gpu.musBuffer.get());
  this->halfKickObjectsLinearKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->halfKickObjectsLinearKernel.setArg(3, gpu.centralBodyIndicesBuffer.get());
  this->halfKickObjectsLinearKernel.setArg(4, sizeof(int), &kernelTotal);

  this->halfKickOrbitalLinearKernel.setArg(0, gpu.positionsBuffer.get());
  this->halfKickOrbitalLinearKernel.setArg(1, gpu.musBuffer.get());
  this->halfKickOrbitalLinearKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->halfKickOrbitalLinearKernel.setArg(3, gpu.centralBodyIndicesBuffer.get());
  this->halfKickOrbitalLinearKernel.setArg(4, sizeof(int), &kernelTotal);

  this->halfKickAngularKernel.setArg(0, gpu.positionsBuffer.get());
  this->halfKickAngularKernel.setArg(1, gpu.musBuffer.get());
  this->halfKickAngularKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->halfKickAngularKernel.setArg(3, gpu.angularVelocitiesBuffer.get());
  this->halfKickAngularKernel.setArg(4, gpu.quadrupoleTensorsBuffer.get());
  this->halfKickAngularKernel.setArg(5, gpu.inertiaTensorsBuffer.get());
  this->halfKickAngularKernel.setArg(6, gpu.orientationsBuffer.get());
  this->halfKickAngularKernel.setArg(7, gpu.meanRadiiBuffer.get());
  this->halfKickAngularKernel.setArg(8, gpu.loveIndicesBuffer.get());
  this->halfKickAngularKernel.setArg(9, gpu.tidalFactorIndicesBuffer.get());
  this->halfKickAngularKernel.setArg(10, gpu.loveNumbersBuffer.get());
  this->halfKickAngularKernel.setArg(11, gpu.tidalFactorsBuffer.get());
  this->halfKickAngularKernel.setArg(12, sizeof(int), &kernelTotal);

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

  this->updateKeplerElementsKernel.setArg(0, gpu.positionsBuffer.get());
  this->updateKeplerElementsKernel.setArg(1, gpu.musBuffer.get());
  this->updateKeplerElementsKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->updateKeplerElementsKernel.setArg(3, gpu.semiAxisesBuffer.get());
  this->updateKeplerElementsKernel.setArg(4, gpu.eccentricitiesBuffer.get());
  this->updateKeplerElementsKernel.setArg(5, gpu.inclinationsBuffer.get());
  this->updateKeplerElementsKernel.setArg(6, gpu.longitudeBuffer.get());
  this->updateKeplerElementsKernel.setArg(7, gpu.periapsisBuffer.get());
  this->updateKeplerElementsKernel.setArg(8, gpu.meanAnomalyBuffer.get());
  this->updateKeplerElementsKernel.setArg(9, gpu.meanMotionBuffer.get());
  this->updateKeplerElementsKernel.setArg(10, gpu.centralBodyIndicesBuffer.get());

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
  this->halfKickObjectsLinearKernel.setArg(5, sizeof(Real), &kickDt);
  this->halfKickOrbitalLinearKernel.setArg(5, sizeof(Real), &kickDt);
  this->halfKickAngularKernel.setArg(13, sizeof(Real), &kickDt);
  this->driftOrbitalLinearKernel.setArg(11, sizeof(Real), &dt);
  this->updateKeplerElementsKernel.setArg(11, sizeof(Real), &dt);
  this->driftObjectsLinearKernel.setArg(2, sizeof(Real), &dt);
  this->driftAngularKernel.setArg(2, sizeof(Real), &dt);
}

// Constructor
template <typename Real>
WisdomHolmanIntegratorGPU<Real>::WisdomHolmanIntegratorGPU(ResourceManager &resourceManager)
    : driftAngularKernel(resourceManager.GetKernel(Res::WH_DRIFT_ANGULAR_KERNEL)),
      driftObjectsLinearKernel(resourceManager.GetKernel(Res::WH_DRIFT_OBJECTS_LINEAR_KERNEL)),
      driftOrbitalLinearKernel(resourceManager.GetKernel(Res::WH_DRIFT_ORBITAL_LINEAR_KERNEL)),
      halfKickObjectsLinearKernel(resourceManager.GetKernel(Res::WH_HALF_KICK_OBJECTS_LINEAR_KERNEL)),
      halfKickOrbitalLinearKernel(resourceManager.GetKernel(Res::WH_HALF_KICK_ORBITAL_LINEAR_KERNEL)),
      halfKickAngularKernel(resourceManager.GetKernel(Res::WH_HALF_KICK_ANGULAR_KERNEL)),
      halfKickKernel(resourceManager.GetKernel(Res::WH_HALF_KICK_KERNEL)),
      updateKeplerElementsKernel(resourceManager.GetKernel(Res::WH_UPDATE_KEPLER_ELEMENTS_KERNEL))
{
}

// Public functions
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::init(AllIntegratorGPUBuffers &gpu, Total &total)
{
  WisdomHolmanGPUBuffers whGPU(gpu);
  this->initKernels(whGPU, total);
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
  queue.enqueueNDKernelBuffer(this->halfKickOrbitalLinearKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickObjectsLinearKernel.get(), 1, &total.orbital, &total.object);
  queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &total.total);

  // Update Kepler Elements
  queue.enqueueNDKernelBuffer(this->updateKeplerElementsKernel.get(), 1, NULL, &total.orbital);

  // Drift
  queue.enqueueNDKernelBuffer(this->driftObjectsLinearKernel.get(), 1, &total.orbital, &total.object);
  queue.enqueueNDKernelBuffer(this->driftOrbitalLinearKernel.get(), 1, NULL, &total.orbital);
  queue.enqueueNDKernelBuffer(this->driftAngularKernel.get(), 1, NULL, &total.total);

  // Kick
  // this->queue.enqueueNDKernelBuffer(this->halfKickKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickOrbitalLinearKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->halfKickObjectsLinearKernel.get(), 1, &total.orbital, &total.object);
  queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &total.total);
}