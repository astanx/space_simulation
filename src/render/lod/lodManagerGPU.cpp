#include "render/lod/manager/lodManagerGPU.h"

#include "render/lod/lodSettings.h"

#include "render/frustum.h"

#include "compute/context.h"

#include "camera/camera.h"

#include "scene/frameContext.h"

#include "resources/resources.h"
#include "resources/resourceManager.h"
#include "resources/gpuTypes.h"

#include "physics/world/total.h"

// Private functions
void LODManagerGPU::initBuffers(Context &ctx, CommandQueue &queue, size_t totalObjects)
{
  cl_context context = ctx.get();
  this->fullScan.initBuffers(context, queue, totalObjects, this->groupCount);
  this->nonFullScan.initBuffers(context, queue, totalObjects, this->groupCount);
  this->impostorScan.initBuffers(context, queue, totalObjects, this->groupCount);
  this->pointScan.initBuffers(context, queue, totalObjects, this->groupCount);
  queue.finish();
}

void LODManagerGPU::initKernels(LODGPUData &data, LODSettings &settings, size_t totalObjects)
{
  this->lodPassKernel.setArg(0, this->getIsFullBuffer().get());
  this->lodPassKernel.setArg(1, this->getIsNonFullBuffer().get());
  this->lodPassKernel.setArg(2, this->getIsImpostorBuffer().get());
  this->lodPassKernel.setArg(3, this->getIsPointBuffer().get());
  this->lodPassKernel.setArg(4, data.positions.get());
  this->lodPassKernel.setArg(5, data.meanRadii.get());
  this->lodPassKernel.setArg(6, data.modelImportances.get());
  this->lodPassKernel.setArg(7, data.modelRangeStart.get());
  this->lodPassKernel.setArg(8, data.modelRangeEnd.get());
  this->lodPassKernel.setArg(9, data.isNonFullable.get());
  this->lodPassKernel.setArg(10, sizeof(data.rangeCount), &data.rangeCount);
  this->lodPassKernel.setArg(13, sizeof(settings.baseMinPixelSize), &settings.baseMinPixelSize);
  this->lodPassKernel.setArg(14, sizeof(settings.fullThreshold), &settings.fullThreshold);
  this->lodPassKernel.setArg(15, sizeof(settings.impostorThreshold), &settings.impostorThreshold);
  this->lodPassKernel.setArg(16, sizeof(uint32_t), &totalObjects);

  this->fullScan.initKernels(totalObjects, this->localScanSize, this->groupCount);
  this->nonFullScan.initKernels(totalObjects, this->localScanSize, this->groupCount);
  this->impostorScan.initKernels(totalObjects, this->localScanSize, this->groupCount);
  this->pointScan.initKernels(totalObjects, this->localScanSize, this->groupCount);
}

void LODManagerGPU::updateKernels(const Camera &camera, FrameContext &ctx)
{
  Frustum frustum = camera.getFrustum(ctx.aspect);
  float fov = camera.getFOV();
  // size fix here
  Vec3<float> camPos = static_cast<Vec3<float>>(camera.getPosition());

  this->lodPassKernel.setArg(11, sizeof(fov), &fov);
  this->lodPassKernel.setArg(12, sizeof(ctx.height), &ctx.height);
  this->lodPassKernel.setArg(17, sizeof(frustum.faces), &frustum.faces);
  this->lodPassKernel.setArg(18, sizeof(camPos), &camPos);
}

// Constructor
LODManagerGPU::LODManagerGPU(ResourceManager &resourceManager)
    : fullScan(resourceManager, Res::LOD_FULL_LOCAL_SCAN_KERNEL, Res::LOD_FULL_GROUP_SCAN_KERNEL, Res::LOD_FULL_GROUP_OFFSET_SCAN_KERNEL),
      nonFullScan(resourceManager, Res::LOD_FULL_LOCAL_SCAN_KERNEL, Res::LOD_FULL_GROUP_SCAN_KERNEL, Res::LOD_FULL_GROUP_OFFSET_SCAN_KERNEL),
      impostorScan(resourceManager, Res::LOD_IMPOSTOR_LOCAL_SCAN_KERNEL, Res::LOD_IMPOSTOR_GROUP_SCAN_KERNEL, Res::LOD_IMPOSTOR_GROUP_OFFSET_SCAN_KERNEL),
      pointScan(resourceManager, Res::LOD_POINT_LOCAL_SCAN_KERNEL, Res::LOD_POINT_GROUP_SCAN_KERNEL, Res::LOD_POINT_GROUP_OFFSET_SCAN_KERNEL),
      lodPassKernel(resourceManager.GetKernel(Res::LOD_PASS_KERNEL)) {};

// Public functions
void LODManagerGPU::init(Context &ctx, CommandQueue &queue, LODGPUData &data, LODSettings &settings, size_t totalObjects)
{
  this->groupCount = (totalObjects + this->localScanSize - 1) / this->localScanSize;

  this->initBuffers(ctx, queue, totalObjects);
  this->initKernels(data, settings, totalObjects);
};

void LODManagerGPU::update(CommandQueue &queue, const Camera &camera, FrameContext &ctx, size_t totalObjects)
{
  this->updateKernels(camera, ctx);

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
