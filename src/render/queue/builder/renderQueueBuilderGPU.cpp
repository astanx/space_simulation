#include "render/queue/builder/renderQueueBuilderGPU.h"

#include "render/queue/renderQueue.h"

#include "render/queue/data/renderQueueGPUBuffers.h"

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

void RenderQueueBuilderGPU::initKernels(RenderQueueGPUBuffers &data, LODSettings &settings)
{
  this->partitionObjectsKernel.setArg(0, data.data.fullInstances.get());
  this->partitionObjectsKernel.setArg(1, data.data.impostorInstances.get());
  this->partitionObjectsKernel.setArg(2, data.data.pointInstances.get());
  this->partitionObjectsKernel.setArg(3, data.lodData.isFullBuffer.get());
  this->partitionObjectsKernel.setArg(4, data.lodData.isNonFullBuffer.get());
  this->partitionObjectsKernel.setArg(5, data.lodData.isImpostorBuffer.get());
  this->partitionObjectsKernel.setArg(6, data.lodData.isPointBuffer.get());
  this->partitionObjectsKernel.setArg(7, data.lodData.fullOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(8, data.lodData.nonFullOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(9, data.lodData.impostorOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(10, data.lodData.pointOffsetBuffer.get());
  this->partitionObjectsKernel.setArg(11, data.data.positions.get());
  this->partitionObjectsKernel.setArg(12, data.data.orientations.get());
  this->partitionObjectsKernel.setArg(13, data.data.meanRadii.get());
  this->partitionObjectsKernel.setArg(14, data.data.polarRadii.get());
  this->partitionObjectsKernel.setArg(15, data.data.equatorianRadii.get());
  this->partitionObjectsKernel.setArg(16, data.data.modelColors.get());
  this->partitionObjectsKernel.setArg(17, data.data.modelTextureLayers.get());
  this->partitionObjectsKernel.setArg(18, data.data.modelImportances.get());
  this->partitionObjectsKernel.setArg(19, data.data.modelRangeStart.get());
  this->partitionObjectsKernel.setArg(20, data.data.modelRangeEnd.get());
  this->partitionObjectsKernel.setArg(21, this->modelFullCountBuffer.get());
  this->partitionObjectsKernel.setArg(22, this->impostorCountBuffer.get());
  this->partitionObjectsKernel.setArg(23, this->pointCountBuffer.get());
  this->partitionObjectsKernel.setArg(24, sizeof(data.data.rangeCount), &data.data.rangeCount);
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

// Constructor
RenderQueueBuilderGPU::RenderQueueBuilderGPU(ResourceManager &resourceManager) : partitionObjectsKernel(resourceManager.GetKernel(Res::PARTITION_OBJECTS_KERNEL))
{
}

void RenderQueueBuilderGPU::init(Context &ctx, CommandQueue &queue, RenderQueueGPUBuffers &data, LODSettings &settings, size_t modelCount)
{
  this->initBuffers(ctx, queue, modelCount);
  this->initKernels(data, settings);
}