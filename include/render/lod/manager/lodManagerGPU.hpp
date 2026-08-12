#pragma once

#include "camera/camera.h"

#include "compute/commandQueue.h"
#include "compute/kernel.h"

#include "resources/gpuTypes.h"

#include "scene/frameContext.h"

#include "render/frustum.h"

// Private functions
template <typename Real>
void LODManagerGPU::updateKernels(const Camera &camera, FrameContext &ctx)
{
  Frustum frustum = camera.getFrustum(ctx.aspect);
  float fov = camera.getFOV();
  Vec3<Real> camPos = static_cast<Vec3<Real>>(camera.getPosition());

  this->lodPassKernel.setArg(11, sizeof(fov), &fov);
  this->lodPassKernel.setArg(12, sizeof(ctx.height), &ctx.height);
  this->lodPassKernel.setArg(17, sizeof(frustum.faces), &frustum.faces);
  this->lodPassKernel.setArg(18, sizeof(camPos), &camPos);
}

// Public functions
template <typename Real>
void LODManagerGPU::update(CommandQueue &queue, const Camera &camera, FrameContext &ctx, size_t totalObjects)
{
  this->updateKernels<Real>(camera, ctx);

  // Pass
  queue.enqueueNDKernelBuffer(this->lodPassKernel.get(), 1, NULL, &totalObjects);
  queue.finish();

  size_t globalObjects = ((totalObjects + localScanSize - 1) / localScanSize) * localScanSize;
  size_t groupsGlobal = ((this->groupCount + this->localScanSize - 1) / this->localScanSize) * this->localScanSize;

  // Scan for each
  this->fullScan.enqueueKernels(queue, globalObjects, groupsGlobal, this->localScanSize);
  this->nonFullScan.enqueueKernels(queue, globalObjects, groupsGlobal, this->localScanSize);
  this->impostorScan.enqueueKernels(queue, globalObjects, groupsGlobal, this->localScanSize);
  this->pointScan.enqueueKernels(queue, globalObjects, groupsGlobal, this->localScanSize);
}