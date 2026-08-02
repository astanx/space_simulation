#include "render/lod/manager/lodManagerGPU.h"

#include "render/lod/lodSettings.h"

#include "render/frustum.h"

#include "compute/context.h"

#include "camera/camera.h"

#include "scene/frameContext.h"

#include "resources/resources.h"
#include "resources/resourceManager.h"

// Private functions
void LODManagerGPU::initBuffers(Context &ctx, CommandQueue &queue, size_t totalObjects)
{
  this->isFullBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->isImpostorBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->isPointBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);

  this->fullOffsetBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->impostorOffsetBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->pointOffsetBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);

  uint32_t zero = 0;
  cl_event event;
  queue.enqueueFillBuffer(this->isFullBuffer.get(), &zero, sizeof(zero), totalObjects * sizeof(uint32_t));
  queue.enqueueFillBuffer(this->isImpostorBuffer.get(), &zero, sizeof(zero), totalObjects * sizeof(uint32_t));
  queue.enqueueFillBuffer(this->isPointBuffer.get(), &zero, sizeof(zero), totalObjects * sizeof(uint32_t));

  queue.enqueueFillBuffer(this->fullOffsetBuffer.get(), &zero, sizeof(zero), totalObjects * sizeof(uint32_t));
  queue.enqueueFillBuffer(this->impostorOffsetBuffer.get(), &zero, sizeof(zero), totalObjects * sizeof(uint32_t));
  queue.enqueueFillBuffer(this->pointOffsetBuffer.get(), &zero, sizeof(zero), totalObjects * sizeof(uint32_t), &event);

  clWaitForEvents(1, &event);
  clReleaseEvent(event);
}

void LODManagerGPU::initKernels(LODGPUData &data, LODSettings &settings)
{
  this->lodPassKernel.setArg(0, this->isFullBuffer.get());
  this->lodPassKernel.setArg(1, this->isImpostorBuffer.get());
  this->lodPassKernel.setArg(2, this->isPointBuffer.get());
  this->lodPassKernel.setArg(3, data.positions.get());
  this->lodPassKernel.setArg(4, data.meanRadii.get());
  this->lodPassKernel.setArg(5, data.instanceImportances.get());
  this->lodPassKernel.setArg(8, sizeof(settings.baseMinPixelSize), &settings.baseMinPixelSize);
  this->lodPassKernel.setArg(9, sizeof(settings.pixelRadiusThreshold[0]), &settings.pixelRadiusThreshold[0]);
  this->lodPassKernel.setArg(10, sizeof(settings.pixelRadiusThreshold[1]), &settings.pixelRadiusThreshold[1]);

  this->lodPartitionKernel.setArg(0, data.fullInstances.get());
  this->lodPartitionKernel.setArg(1, data.impostorInstances.get());
  this->lodPartitionKernel.setArg(2, data.pointInstances.get());
  this->lodPartitionKernel.setArg(3, this->isFullBuffer.get());
  this->lodPartitionKernel.setArg(4, this->isImpostorBuffer.get());
  this->lodPartitionKernel.setArg(5, this->isPointBuffer.get());
  this->lodPartitionKernel.setArg(6, this->fullOffsetBuffer.get());
  this->lodPartitionKernel.setArg(7, this->impostorOffsetBuffer.get());
  this->lodPartitionKernel.setArg(8, this->pointOffsetBuffer.get());
  this->lodPartitionKernel.setArg(9, data.positions.get());
  this->lodPartitionKernel.setArg(10, data.orientations.get());
  this->lodPartitionKernel.setArg(11, data.meanRadii.get());
  this->lodPartitionKernel.setArg(12, data.polarRadii.get());
  this->lodPartitionKernel.setArg(13, data.equatorianRadii.get());
  this->lodPartitionKernel.setArg(14, data.instanceColors.get());
  this->lodPartitionKernel.setArg(15, data.instanceTextureLayers.get());
  this->lodPartitionKernel.setArg(16, data.instanceImportances.get());
  this->lodPartitionKernel.setArg(17, data.modelRangeStart.get());
  this->lodPartitionKernel.setArg(18, data.modelRangeEnd.get());
  this->lodPartitionKernel.setArg(19, data.modelFullCount.get());
  this->lodPartitionKernel.setArg(20, sizeof(data.rangeCount), &data.rangeCount);

  this->lodPartitionKernel.setArg(23, sizeof(settings.baseMinPixelSize), &settings.baseMinPixelSize);

  this->fullScanKernel.setArg(0, this->isFullBuffer.get());
  this->fullScanKernel.setArg(1, this->fullOffsetBuffer.get());
  this->fullScanKernel.setArg(2, this->localScanSize * sizeof(uint32_t), nullptr);

  this->impostorScanKernel.setArg(0, this->isImpostorBuffer.get());
  this->impostorScanKernel.setArg(1, this->impostorOffsetBuffer.get());
  this->impostorScanKernel.setArg(2, this->localScanSize * sizeof(uint32_t), nullptr);

  this->pointScanKernel.setArg(0, this->isPointBuffer.get());
  this->pointScanKernel.setArg(1, this->pointOffsetBuffer.get());
  this->pointScanKernel.setArg(2, this->localScanSize * sizeof(uint32_t), nullptr);
}

void LODManagerGPU::updateKernels(const Camera &camera, FrameContext &ctx)
{
  Frustum frustum = camera.getFrustum(ctx.aspect);
  float fov = camera.getFOV();
  this->lodPassKernel.setArg(6, sizeof(fov), &fov);
  this->lodPassKernel.setArg(7, sizeof(ctx.height), &ctx.height);
  this->lodPassKernel.setArg(11, sizeof(frustum.faces), &frustum.faces);

  this->lodPartitionKernel.setArg(21, sizeof(fov), &fov);
  this->lodPartitionKernel.setArg(22, sizeof(ctx.height), &ctx.height);
}

// Constructor
LODManagerGPU::LODManagerGPU(ResourceManager &resourceManager)
    : fullScanKernel(resourceManager.GetKernel(Res::LOD_FULL_SCAN_KERNEL)),
      impostorScanKernel(resourceManager.GetKernel(Res::LOD_IMPOSTOR_SCAN_KERNEL)),
      pointScanKernel(resourceManager.GetKernel(Res::LOD_POINT_SCAN_KERNEL)),
      lodPassKernel(resourceManager.GetKernel(Res::LOD_PASS_KERNEL)),
      lodPartitionKernel(resourceManager.GetKernel(Res::LOD_PARTITION_OBJECTS_KERNEL)) {};

// Public functions
void LODManagerGPU::init(Context &ctx, CommandQueue &queue, LODGPUData &data, LODSettings &settings, size_t totalObjects)
{
  this->initBuffers(ctx, queue, totalObjects);

  uint32_t zero = 0;
  cl_event event;
  queue.enqueueFillBuffer(data.modelFullCount.get(), &zero, sizeof(zero), data.rangeCount * sizeof(uint32_t), &event);
  clWaitForEvents(1, &event);
  clReleaseEvent(event);

  this->initKernels(data, settings);
};