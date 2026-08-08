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
  this->isFullBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->isImpostorBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->isPointBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);

  this->fullOffsetBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->impostorOffsetBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->pointOffsetBuffer.init(ctx.get(), CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);

  this->fullGroupSumsBuffer.init(ctx.get(), CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);
  this->impostorGroupSumsBuffer.init(ctx.get(), CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);
  this->pointGroupSumsBuffer.init(ctx.get(), CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);

  this->fullGroupOffsetsBuffer.init(ctx.get(), CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);
  this->impostorGroupOffsetsBuffer.init(ctx.get(), CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);
  this->pointGroupOffsetsBuffer.init(ctx.get(), CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);

  std::vector<uint32_t> zeros(totalObjects, 0);
  queue.enqueueWriteBuffer(this->isFullBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());
  queue.enqueueWriteBuffer(this->isImpostorBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());
  queue.enqueueWriteBuffer(this->isPointBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());

  queue.enqueueWriteBuffer(this->fullOffsetBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());
  queue.enqueueWriteBuffer(this->impostorOffsetBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());
  queue.enqueueWriteBuffer(this->pointOffsetBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());

  std::vector<uint32_t> zerosGroup(groupCount, 0);
  queue.enqueueWriteBuffer(this->fullGroupSumsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
  queue.enqueueWriteBuffer(this->impostorGroupSumsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
  queue.enqueueWriteBuffer(this->pointGroupSumsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());

  queue.enqueueWriteBuffer(this->fullGroupOffsetsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
  queue.enqueueWriteBuffer(this->impostorGroupOffsetsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
  queue.enqueueWriteBuffer(this->pointGroupOffsetsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
  queue.finish();
}

void LODManagerGPU::initKernels(LODGPUData &data, LODSettings &settings, size_t totalObjects)
{
  this->lodPassKernel.setArg(0, this->isFullBuffer.get());
  this->lodPassKernel.setArg(1, this->isImpostorBuffer.get());
  this->lodPassKernel.setArg(2, this->isPointBuffer.get());
  this->lodPassKernel.setArg(3, data.positions.get());
  this->lodPassKernel.setArg(4, data.meanRadii.get());
  this->lodPassKernel.setArg(5, data.instanceImportances.get());
  this->lodPassKernel.setArg(8, sizeof(settings.baseMinPixelSize), &settings.baseMinPixelSize);
  this->lodPassKernel.setArg(9, sizeof(settings.fullThreshold), &settings.fullThreshold);
  this->lodPassKernel.setArg(10, sizeof(settings.impostorThreshold), &settings.impostorThreshold);
  this->lodPassKernel.setArg(11, sizeof(uint32_t), &totalObjects);

  this->fullLocalScanKernel.setArg(0, this->isFullBuffer.get());
  this->fullLocalScanKernel.setArg(1, this->fullOffsetBuffer.get());
  this->fullLocalScanKernel.setArg(2, this->fullGroupSumsBuffer.get());
  this->fullLocalScanKernel.setArg(3, this->localScanSize * sizeof(uint32_t), nullptr);
  this->fullLocalScanKernel.setArg(4, sizeof(uint32_t), &totalObjects);

  this->impostorLocalScanKernel.setArg(0, this->isImpostorBuffer.get());
  this->impostorLocalScanKernel.setArg(1, this->impostorOffsetBuffer.get());
  this->impostorLocalScanKernel.setArg(2, this->impostorGroupSumsBuffer.get());
  this->impostorLocalScanKernel.setArg(3, this->localScanSize * sizeof(uint32_t), nullptr);
  this->impostorLocalScanKernel.setArg(4, sizeof(uint32_t), &totalObjects);

  this->pointLocalScanKernel.setArg(0, this->isPointBuffer.get());
  this->pointLocalScanKernel.setArg(1, this->pointOffsetBuffer.get());
  this->pointLocalScanKernel.setArg(2, this->pointGroupSumsBuffer.get());
  this->pointLocalScanKernel.setArg(3, this->localScanSize * sizeof(uint32_t), nullptr);
  this->pointLocalScanKernel.setArg(4, sizeof(uint32_t), &totalObjects);

  this->fullGroupScanKernel.setArg(0, this->fullGroupSumsBuffer.get());
  this->fullGroupScanKernel.setArg(1, this->fullGroupOffsetsBuffer.get());
  this->fullGroupScanKernel.setArg(2, this->localScanSize * sizeof(uint32_t), nullptr);
  this->fullGroupScanKernel.setArg(3, sizeof(uint32_t), &this->groupCount);

  this->impostorGroupScanKernel.setArg(0, this->impostorGroupSumsBuffer.get());
  this->impostorGroupScanKernel.setArg(1, this->impostorGroupOffsetsBuffer.get());
  this->impostorGroupScanKernel.setArg(2, this->localScanSize * sizeof(uint32_t), nullptr);
  this->impostorGroupScanKernel.setArg(3, sizeof(uint32_t), &this->groupCount);

  this->pointGroupScanKernel.setArg(0, this->pointGroupSumsBuffer.get());
  this->pointGroupScanKernel.setArg(1, this->pointGroupOffsetsBuffer.get());
  this->pointGroupScanKernel.setArg(2, this->localScanSize * sizeof(uint32_t), nullptr);
  this->pointGroupScanKernel.setArg(3, sizeof(uint32_t), &this->groupCount);

  this->fullGroupOffsetScanKernel.setArg(0, this->fullOffsetBuffer.get());
  this->fullGroupOffsetScanKernel.setArg(1, this->fullGroupOffsetsBuffer.get());
  this->fullGroupOffsetScanKernel.setArg(2, sizeof(uint32_t), &totalObjects);

  this->impostorGroupOffsetScanKernel.setArg(0, this->impostorOffsetBuffer.get());
  this->impostorGroupOffsetScanKernel.setArg(1, this->impostorGroupOffsetsBuffer.get());
  this->impostorGroupOffsetScanKernel.setArg(2, sizeof(uint32_t), &totalObjects);

  this->pointGroupOffsetScanKernel.setArg(0, this->pointOffsetBuffer.get());
  this->pointGroupOffsetScanKernel.setArg(1, this->pointGroupOffsetsBuffer.get());
  this->pointGroupOffsetScanKernel.setArg(2, sizeof(uint32_t), &totalObjects);
}

void LODManagerGPU::updateKernels(const Camera &camera, FrameContext &ctx)
{
  Frustum frustum = camera.getFrustum(ctx.aspect);
  float fov = camera.getFOV();
  //size fix here
  Vec3<float> camPos = static_cast<Vec3<float>>(camera.getPosition());

  this->lodPassKernel.setArg(6, sizeof(fov), &fov);
  this->lodPassKernel.setArg(7, sizeof(ctx.height), &ctx.height);
  this->lodPassKernel.setArg(12, sizeof(frustum.faces), &frustum.faces);
  this->lodPassKernel.setArg(13, sizeof(camPos), &camPos);
}

// Constructor
LODManagerGPU::LODManagerGPU(ResourceManager &resourceManager)
    : fullLocalScanKernel(resourceManager.GetKernel(Res::LOD_FULL_LOCAL_SCAN_KERNEL)),
      impostorLocalScanKernel(resourceManager.GetKernel(Res::LOD_IMPOSTOR_LOCAL_SCAN_KERNEL)),
      pointLocalScanKernel(resourceManager.GetKernel(Res::LOD_POINT_LOCAL_SCAN_KERNEL)),
      fullGroupScanKernel(resourceManager.GetKernel(Res::LOD_FULL_GROUP_SCAN_KERNEL)),
      impostorGroupScanKernel(resourceManager.GetKernel(Res::LOD_IMPOSTOR_GROUP_SCAN_KERNEL)),
      pointGroupScanKernel(resourceManager.GetKernel(Res::LOD_POINT_GROUP_SCAN_KERNEL)),
      fullGroupOffsetScanKernel(resourceManager.GetKernel(Res::LOD_FULL_GROUP_OFFSET_SCAN_KERNEL)),
      impostorGroupOffsetScanKernel(resourceManager.GetKernel(Res::LOD_IMPOSTOR_GROUP_OFFSET_SCAN_KERNEL)),
      pointGroupOffsetScanKernel(resourceManager.GetKernel(Res::LOD_POINT_GROUP_OFFSET_SCAN_KERNEL)),
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

  // std::cout << "TOTAL: " << totalObjects << std::endl;

  // Pass
  queue.enqueueNDKernelBuffer(this->lodPassKernel.get(), 1, NULL, &totalObjects);
  queue.finish();

  size_t globalObjects = ((totalObjects + localScanSize - 1) / localScanSize) * localScanSize;

  // std::cout << "GLOBAL: " << globalObjects << std::endl;
  size_t groupsGlobal = ((this->groupCount + this->localScanSize - 1) / this->localScanSize) * this->localScanSize;
  // std::cout << "GROUP GLOBAL: " << groupsGlobal << std::endl;
  // std::cout << "GROUP COUNT: " << groupCount << std::endl;

  // std::vector<uint32_t> fullCount(totalObjects);

  // queue.enqueueReadBuffer(
  //     this->isFullBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     totalObjects * sizeof(uint32_t),
  //     fullCount.data());

  // std::vector<uint32_t> impostorCount(totalObjects);

  // queue.enqueueReadBuffer(
  //     this->isImpostorBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     totalObjects * sizeof(uint32_t),
  //     impostorCount.data());

  // std::vector<uint32_t> pointCount(totalObjects);

  // queue.enqueueReadBuffer(
  //     this->isPointBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     totalObjects * sizeof(uint32_t),
  //     pointCount.data());

  // uint32_t cf = 0;
  // uint32_t ci = 0;
  // uint32_t cp = 0;
  // for (size_t i = 0; i < totalObjects; i++)
  // {
  //   cf += fullCount[i];
  //   ci += impostorCount[i];
  //   cp += pointCount[i];
  // }

  // std::cout << "FULL COUNT = " << cf << "\n";
  // std::cout << "IMPOSTOR COUNT = " << ci << "\n";
  // std::cout << "POINT COUNT = " << cp << "\n";

  // Scan for each
  queue.enqueueNDKernelBuffer(this->fullLocalScanKernel.get(), 1, NULL, &globalObjects, &this->localScanSize);
  queue.enqueueNDKernelBuffer(this->impostorLocalScanKernel.get(), 1, NULL, &globalObjects, &this->localScanSize);
  queue.enqueueNDKernelBuffer(this->pointLocalScanKernel.get(), 1, NULL, &globalObjects, &this->localScanSize);
  // queue.finish();

  // std::vector<uint32_t> fullGroupSums(groupCount);

  // queue.enqueueReadBuffer(
  //     this->fullGroupSumsBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     groupCount * sizeof(uint32_t),
  //     fullGroupSums.data());

  // std::vector<uint32_t> impostorGroupSums(groupCount);

  // queue.enqueueReadBuffer(
  //     this->impostorGroupSumsBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     groupCount * sizeof(uint32_t),
  //     impostorGroupSums.data());

  // std::vector<uint32_t> pointGroupSums(groupCount);

  // queue.enqueueReadBuffer(
  //     this->pointGroupSumsBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     groupCount * sizeof(uint32_t),
  //     pointGroupSums.data());

  // uint32_t gsf = 0;
  // uint32_t gsi = 0;
  // uint32_t gsp = 0;
  // for (size_t i = 0; i < groupCount; i++)
  // {
  //   gsf += fullGroupSums[i];
  //   gsi += impostorGroupSums[i];
  //   gsp += pointGroupSums[i];
  // }

  // std::cout << "FULL GROUP SUM = " << gsf << "\n";
  // std::cout << "IMPOSTOR GROUP SUM = " << gsi << "\n";
  // std::cout << "POINT GROUP SUM = " << gsp << "\n";

  queue.enqueueNDKernelBuffer(this->fullGroupScanKernel.get(), 1, NULL, &groupsGlobal, &this->localScanSize);
  queue.enqueueNDKernelBuffer(this->impostorGroupScanKernel.get(), 1, NULL, &groupsGlobal, &this->localScanSize);
  queue.enqueueNDKernelBuffer(this->pointGroupScanKernel.get(), 1, NULL, &groupsGlobal, &this->localScanSize);
  // queue.finish();

  // std::vector<uint32_t> full(totalObjects);

  // queue.enqueueReadBuffer(
  //     this->fullOffsetBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     totalObjects * sizeof(uint32_t),
  //     full.data());

  // std::vector<uint32_t> impostor(totalObjects);

  // queue.enqueueReadBuffer(
  //     this->impostorOffsetBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     totalObjects * sizeof(uint32_t),
  //     impostor.data());

  // std::vector<uint32_t> point(totalObjects);

  // queue.enqueueReadBuffer(
  //     this->pointOffsetBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     totalObjects * sizeof(uint32_t),
  //     point.data());

  // uint32_t of = 0;
  // uint32_t oi = 0;
  // uint32_t op = 0;
  // for (size_t i = 0; i < totalObjects; i++)
  // {

  //   of += full[i];
  //   oi += impostor[i];
  //   op += point[i];

  //   if (i == 0)
  //   {
  //     std::cout << "FULL FIRST OFFSET = " << of << "\n";
  //     std::cout << "IMPOSTOR FIRST OFFSET = " << oi << "\n";
  //     std::cout << "POINT FIRST OFFSET = " << op << "\n";
  //   }
  // }

  // std::cout << "FULL OFFSETS SUM = " << of << "\n";
  // std::cout << "IMPOSTOR OFFSETS SUM = " << oi << "\n";
  // std::cout << "POINT OFFSETS SUM = " << op << "\n";

  queue.enqueueNDKernelBuffer(this->fullGroupOffsetScanKernel.get(), 1, NULL, &globalObjects, &this->localScanSize);
  queue.enqueueNDKernelBuffer(this->impostorGroupOffsetScanKernel.get(), 1, NULL, &globalObjects, &this->localScanSize);
  queue.enqueueNDKernelBuffer(this->pointGroupOffsetScanKernel.get(), 1, NULL, &globalObjects, &this->localScanSize);

  // queue.finish();

  // std::vector<uint32_t> fullGroupOffset(groupCount);

  // queue.enqueueReadBuffer(
  //     this->fullGroupOffsetsBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     groupCount * sizeof(uint32_t),
  //     fullGroupOffset.data());

  // std::vector<uint32_t> impostorGroupOffset(groupCount);

  // queue.enqueueReadBuffer(
  //     this->impostorGroupOffsetsBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     groupCount * sizeof(uint32_t),
  //     impostorGroupOffset.data());

  // std::vector<uint32_t> pointGroupOffset(groupCount);

  // queue.enqueueReadBuffer(
  //     this->pointGroupOffsetsBuffer.get(),
  //     CL_TRUE,
  //     0,
  //     groupCount * sizeof(uint32_t),
  //     pointGroupOffset.data());

  // uint32_t gof = 0;
  // uint32_t goi = 0;
  // uint32_t gop = 0;
  // for (size_t i = 0; i < groupCount; i++)
  // {
  //   gof += fullGroupOffset[i];
  //   goi += impostorGroupOffset[i];
  //   gop += pointGroupOffset[i];
  // }

  // std::cout << "FULL GROUP OFFSET = " << gof << "\n";
  // std::cout << "IMPOSTOR GROUP OFFSET = " << goi << "\n";
  // std::cout << "POINT GROUP OFFSET = " << gop << "\n";
}
