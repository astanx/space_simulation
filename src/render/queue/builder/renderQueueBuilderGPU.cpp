#include "render/queue/builder/renderQueueBuilderGPU.h"

#include "render/queue/renderQueue.h"

#include "render/lod/lodSettings.h"
#include "render/lod/manager/lodManagerGPU.h"

#include "render/instanceManager.h"

#include "camera/camera.h"

#include "scene/frameContext.h"

#include "physics/world/total.h"

#include "compute/kernel.h"
#include "compute/commandQueue.h"
#include "compute/clBuffer.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"
#include "resources/gpuTypes.h"

void RenderQueueBuilderGPU::initKernels(RenderQueueGPUData &data, LODSettings &settings)
{
  this->partitionObjectsKernel.setArg(0, data.fullInstances.get());
  this->partitionObjectsKernel.setArg(1, data.impostorInstances.get());
  this->partitionObjectsKernel.setArg(2, data.pointInstances.get());
  this->partitionObjectsKernel.setArg(3, data.isFullBuffer.get());
  this->partitionObjectsKernel.setArg(4, data.isNonFullBuffer.get());
  this->partitionObjectsKernel.setArg(5, data.isImpostorBuffer.get());
  this->partitionObjectsKernel.setArg(6, data.isPointBuffer.get());
  this->partitionObjectsKernel.setArg(7, data.fullOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(8, data.nonFullOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(9, data.impostorOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(10, data.pointOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(11, data.positions.get());
  this->partitionObjectsKernel.setArg(12, data.orientations.get());
  this->partitionObjectsKernel.setArg(13, data.meanRadii.get());
  this->partitionObjectsKernel.setArg(14, data.polarRadii.get());
  this->partitionObjectsKernel.setArg(15, data.equatorianRadii.get());
  this->partitionObjectsKernel.setArg(16, data.instanceColors.get());
  this->partitionObjectsKernel.setArg(17, data.instanceTextureLayers.get());
  this->partitionObjectsKernel.setArg(18, data.instanceImportances.get());
  this->partitionObjectsKernel.setArg(19, data.modelRangeStart.get());
  this->partitionObjectsKernel.setArg(20, data.modelRangeEnd.get());
  this->partitionObjectsKernel.setArg(21, this->modelFullCountBuffer.get());
  this->partitionObjectsKernel.setArg(22, this->impostorCountBuffer.get());
  this->partitionObjectsKernel.setArg(23, this->pointCountBuffer.get());
  this->partitionObjectsKernel.setArg(24, sizeof(data.rangeCount), &data.rangeCount);
  this->partitionObjectsKernel.setArg(27, sizeof(settings.baseMinPixelSize), &settings.baseMinPixelSize);
}

void RenderQueueBuilderGPU::initBuffers(Context &ctx, CommandQueue &queue, size_t modelCount)
{
  this->modelFullCountBuffer.init(ctx.get(), CL_MEM_READ_WRITE, modelCount * sizeof(uint32_t), nullptr);
  this->impostorCountBuffer.init(ctx.get(), CL_MEM_READ_WRITE, sizeof(uint32_t), nullptr);
  this->pointCountBuffer.init(ctx.get(), CL_MEM_READ_WRITE, sizeof(uint32_t), nullptr);

  std::vector<uint32_t> zerosModel(modelCount, 0);
  std::vector<uint32_t> zero(1, 0);
  queue.enqueueWriteBuffer(this->modelFullCountBuffer.get(), CL_FALSE, 0, modelCount * sizeof(uint32_t), zerosModel.data());
  queue.enqueueWriteBuffer(this->impostorCountBuffer.get(), CL_FALSE, 0, sizeof(uint32_t), zero.data());
  queue.enqueueWriteBuffer(this->pointCountBuffer.get(), CL_FALSE, 0, sizeof(uint32_t), zero.data());
  queue.finish();
}
void RenderQueueBuilderGPU::updateKernels(const Camera &camera, FrameContext &ctx)
{
  float fov = camera.getFOV();
  // size fix here
  Vec3<float> camPos = static_cast<Vec3<float>>(camera.getPosition());
  this->partitionObjectsKernel.setArg(25, sizeof(fov), &fov);
  this->partitionObjectsKernel.setArg(26, sizeof(ctx.height), &ctx.height);
  this->partitionObjectsKernel.setArg(28, sizeof(camPos), &camPos);
}

// Constructor
RenderQueueBuilderGPU::RenderQueueBuilderGPU(ResourceManager &resourceManager) : partitionObjectsKernel(resourceManager.GetKernel(Res::PARTITION_OBJECTS_KERNEL))
{
}

void RenderQueueBuilderGPU::init(Context &ctx, CommandQueue &queue, RenderQueueGPUData &data, LODSettings &settings, size_t modelCount)
{
  this->initBuffers(ctx, queue, modelCount);
  this->initKernels(data, settings);
}

void RenderQueueBuilderGPU::build(CommandQueue &commandQueue, RenderQueue &renderQueue, LODManagerGPU &lod, InstanceManager &instanceManager, const Camera &camera, FrameContext &ctx, std::vector<Model *> &models, size_t totalObjects)
{
  renderQueue.clear();
  instanceManager.clear();

  lod.update(commandQueue, camera, ctx, totalObjects);
  this->updateKernels(camera, ctx);

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
    Range allocation = instanceManager.getAllocation(models[i]);
    if (models[i]->hasFlag(ModelFlags::CastsShadow))
      renderQueue.addShadowBatch({models[i], allocation});

    if (models[i]->hasFlag(ModelFlags::ReflectsLight))
      renderQueue.addReflectorBatch({models[i], allocation});

    if (fullCounts[i] == 0)
      continue;

    Range range;
    range.begin = allocation.begin;
    range.end = allocation.begin + fullCounts[i];

    if (models[i]->getIsTangent())
      renderQueue.addTangentBatch({models[i], range});
    else
      renderQueue.addCoreBatch({models[i], range});
  }
}
