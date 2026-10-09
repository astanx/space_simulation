#pragma once

#include "debug/logger.h"

#include "physics/integrators/integratable.h"
#include "physics/integrators/data/eulerGPUBuffers.h"

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
void EulerIntegratorGPU<Real>::initKernels(EulerGPUBuffers &gpu, Total total)
{
  int kernelTotal = static_cast<int>(total.object + total.orbital);

  this->driftAngularKernel.setArg(0, gpu.positionsBuffer.get());
  this->driftAngularKernel.setArg(1, gpu.musBuffer.get());
  this->driftAngularKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->driftAngularKernel.setArg(3, gpu.angularVelocitiesBuffer.get());
  this->driftAngularKernel.setArg(4, gpu.quadrupoleTensorsBuffer.get());
  this->driftAngularKernel.setArg(5, gpu.inertiaTensorsBuffer.get());
  this->driftAngularKernel.setArg(6, gpu.orientationsBuffer.get());
  this->driftAngularKernel.setArg(7, gpu.meanRadiiBuffer.get());
  this->driftAngularKernel.setArg(8, gpu.loveIndicesBuffer.get());
  this->driftAngularKernel.setArg(9, gpu.tidalFactorIndicesBuffer.get());
  this->driftAngularKernel.setArg(10, gpu.loveNumbersBuffer.get());
  this->driftAngularKernel.setArg(11, gpu.tidalFactorsBuffer.get());
  this->driftAngularKernel.setArg(12, sizeof(int), &kernelTotal);

  this->driftLinearKernel.setArg(0, gpu.positionsBuffer.get());
  this->driftLinearKernel.setArg(1, gpu.musBuffer.get());
  this->driftLinearKernel.setArg(2, gpu.velocitiesBuffer.get());
  this->driftLinearKernel.setArg(3, sizeof(int), &kernelTotal);
}
template <typename Real>
void EulerIntegratorGPU<Real>::updateDt(Real dt)
{
  this->driftLinearKernel.setArg(4, sizeof(Real), &dt);
  this->driftAngularKernel.setArg(13, sizeof(Real), &dt);
}

// Constructor
template <typename Real>
EulerIntegratorGPU<Real>::EulerIntegratorGPU(ResourceManager &resourceManager)
    : driftAngularKernel(resourceManager.GetKernel(Res::EULER_DRIFT_ANGULAR_KERNEL)),
      driftLinearKernel(resourceManager.GetKernel(Res::EULER_DRIFT_LINEAR_KERNEL))
{
}

// Public functions
template <typename Real>
void EulerIntegratorGPU<Real>::init(AllIntegratorGPUBuffers &gpu, Total &total)
{
  EulerGPUBuffers eulerGPU(gpu);
  this->initKernels(eulerGPU, total);
}

template <typename Real>
void EulerIntegratorGPU<Real>::stepReal(CommandQueue &queue, Total &total, Real dt)
{
  this->updateDt(dt);

  // Drift
  queue.enqueueNDKernelBuffer(this->driftLinearKernel.get(), 1, NULL, &total.total);
  queue.enqueueNDKernelBuffer(this->driftAngularKernel.get(), 1, NULL, &total.total);
}