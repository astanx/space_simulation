#pragma once

#include "render/world/data/renderDataGPU.h"

#include "compute/commandQueue.h"
#include "compute/clBuffer.h"

#include <queue>

class Kernel;
class Context;
class ResourceManager;
class Camera;
struct LODSettings;
struct FrameContext;
struct Total;

struct LODGPUData
{
  CLBuffer &positions;
  CLBuffer &meanRadii;
  CLBuffer &instanceImportances;
};

class LODManagerGPU
{
private:
  size_t localScanSize = 128;
  size_t groupCount;

  Kernel &fullLocalScanKernel;
  Kernel &impostorLocalScanKernel;
  Kernel &pointLocalScanKernel;
  Kernel &fullGroupScanKernel;
  Kernel &impostorGroupScanKernel;
  Kernel &pointGroupScanKernel;
  Kernel &fullGroupOffsetScanKernel;
  Kernel &impostorGroupOffsetScanKernel;
  Kernel &pointGroupOffsetScanKernel;
  Kernel &lodPassKernel;

  CLBuffer isFullBuffer;
  CLBuffer isImpostorBuffer;
  CLBuffer isPointBuffer;

  CLBuffer fullOffsetBuffer;
  CLBuffer impostorOffsetBuffer;
  CLBuffer pointOffsetBuffer;

  CLBuffer fullGroupSumsBuffer;
  CLBuffer impostorGroupSumsBuffer;
  CLBuffer pointGroupSumsBuffer;

  CLBuffer fullGroupOffsetsBuffer;
  CLBuffer impostorGroupOffsetsBuffer;
  CLBuffer pointGroupOffsetsBuffer;

  std::queue<cl_event> events;

  void initBuffers(Context &ctx, CommandQueue &queue, size_t totalObjects);
  void initKernels(LODGPUData &data, LODSettings &settings, size_t totalObjects);
  void updateKernels(const Camera &camera, FrameContext &ctx);

public:
  LODManagerGPU(ResourceManager &resourceManager);
  ~LODManagerGPU() = default;

  void init(Context &ctx, CommandQueue &queue, LODGPUData &data, LODSettings &settings, size_t totalObjects);
  void update(CommandQueue &queue, const Camera& camera, FrameContext& ctx, size_t totalObjects);

  CLBuffer &getIsFullBuffer() { return this->isFullBuffer; };
  CLBuffer &getIsImpostorBuffer() { return this->isImpostorBuffer; };
  CLBuffer &getIsPointBuffer() { return this->isPointBuffer; };

  CLBuffer &getFullOffsetBuffer() { return this->fullOffsetBuffer; };
  CLBuffer &getImpostorOffsetBuffer() { return this->impostorOffsetBuffer; };
  CLBuffer &getPointOffsetBuffer() { return this->pointOffsetBuffer; };
};