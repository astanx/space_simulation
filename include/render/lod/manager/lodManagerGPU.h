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

struct LODGPUData
{
  CLBuffer &positions;
  CLBuffer &orientations;
  CLBuffer &meanRadii;
  CLBuffer &polarRadii;
  CLBuffer &equatorianRadii;
  CLBuffer &instanceImportances;
  CLBuffer &instanceColors;
  CLBuffer &instanceTextureLayers;
  CLBuffer &fullInstances;
  CLBuffer &impostorInstances;
  CLBuffer &pointInstances;
  CLBuffer &modelRangeStart;
  CLBuffer &modelRangeEnd;
  CLBuffer &modelFullCount;

  uint32_t rangeCount;
};

class LODManagerGPU
{
private:
  size_t localScanSize = 128;

  CommandQueue queue;

  Kernel &fullScanKernel;
  Kernel &impostorScanKernel;
  Kernel &pointScanKernel;
  Kernel &lodPassKernel;
  Kernel &lodPartitionKernel;

  CLBuffer isFullBuffer;
  CLBuffer isImpostorBuffer;
  CLBuffer isPointBuffer;

  CLBuffer fullOffsetBuffer;
  CLBuffer impostorOffsetBuffer;
  CLBuffer pointOffsetBuffer;

  std::queue<cl_event> events;

  void initQueue(Context &ctx);
  void initBuffers(Context &ctx, size_t totalObjects);
  void initKernels(LODGPUData &data, LODSettings &settings);
  void updateKernels(const Camera &camera, FrameContext &ctx);

public:
  LODManagerGPU(ResourceManager &resourceManager);
  ~LODManagerGPU() = default;

  void init(Context &ctx, LODGPUData &data, LODSettings &settings, size_t totalObjects);
};