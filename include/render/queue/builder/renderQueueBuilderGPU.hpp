#pragma once

#include "render/queue/renderQueue.h"
#include "render/frustum.h"
#include "render/instanceManager.h"
#include "render/lod/manager/lodManagerGPU.h"

#include "camera/camera.h"

#include "compute/commandQueue.h"
#include "compute/kernel.h"

#include "resources/gpuTypes.h"

#include "scene/frameContext.h"

#include "graphics/model.h"

// Private functions
template <typename Real>
void RenderQueueBuilderGPU::updateKernels(const Camera &camera, FrameContext &ctx)
{
  float fov = camera.getFOV();
  Vec3<Real> camPos = static_cast<Vec3<Real>>(camera.getPosition());
  this->partitionObjectsKernel.setArg(25, sizeof(fov), &fov);
  this->partitionObjectsKernel.setArg(26, sizeof(ctx.height), &ctx.height);
  this->partitionObjectsKernel.setArg(28, sizeof(camPos), &camPos);
}

// Public functions
template <typename Real>
void RenderQueueBuilderGPU::build(CommandQueue &commandQueue, RenderQueue &renderQueue, LODManagerGPU &lod, InstanceManager &instanceManager, const Camera &camera, FrameContext &ctx, const std::vector<Model *> &models, size_t totalObjects)
{
  renderQueue.clear();
  instanceManager.clear();

  lod.update<Real>(commandQueue, camera, ctx, totalObjects);
  this->updateKernels<Real>(camera, ctx);

  if (this->events.size() >= 2)
  {
    cl_event event = this->events.front();
    this->events.pop();
    clWaitForEvents(1, &event);
    clReleaseEvent(event);
  }

  commandQueue.enqueueAcquireGLBuffer(instanceManager.getFullInstancesBuffer().get());
  commandQueue.enqueueAcquireGLBuffer(instanceManager.getImpostorInstancesBuffer().get());
  commandQueue.enqueueAcquireGLBuffer(instanceManager.getPointInstancesBuffer().get());

  commandQueue.enqueueNDKernelBuffer(this->partitionObjectsKernel.get(), 1, NULL, &totalObjects);

  commandQueue.enqueueReleaseGLBuffer(instanceManager.getFullInstancesBuffer().get());
  commandQueue.enqueueReleaseGLBuffer(instanceManager.getImpostorInstancesBuffer().get());
  commandQueue.enqueueReleaseGLBuffer(instanceManager.getPointInstancesBuffer().get());

  std::vector<uint32_t> fullCounts(models.size());
  std::vector<uint32_t> impostorCounts(1);
  std::vector<uint32_t> pointCounts(1);
  cl_event lastEvent;
  commandQueue.enqueueReadBuffer(this->modelFullCountBuffer.get(), CL_TRUE, 0, models.size() * sizeof(uint32_t), fullCounts.data());
  commandQueue.enqueueReadBuffer(this->impostorCountBuffer.get(), CL_TRUE, 0, sizeof(uint32_t), impostorCounts.data());
  commandQueue.enqueueReadBuffer(this->pointCountBuffer.get(), CL_TRUE, 0, sizeof(uint32_t), pointCounts.data(), &lastEvent);

  this->events.push(lastEvent);
  instanceManager.setImpostorCount(impostorCounts[0]);
  instanceManager.setPointCount(pointCounts[0]);

  for (size_t i = 0; i < models.size(); i++)
  {
    if (fullCounts[i] == 0)
      continue;

    Range allocation = instanceManager.getAllocation(models[i]);
    Range range;
    range.begin = allocation.begin;
    range.end = allocation.begin + fullCounts[i];

    if (models[i]->getIsTangent())
      renderQueue.addTangentBatch({models[i], range});
    else
      renderQueue.addCoreBatch({models[i], range});
  }
}
