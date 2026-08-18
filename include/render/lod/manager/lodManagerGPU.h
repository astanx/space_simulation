#pragma once

#include "render/lod/manager/scan.h"

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

struct LODGPUBuffers
{
  CLBuffer &positions;
  CLBuffer &meanRadii;
  CLBuffer &modelImportances;
  CLBuffer &modelRangeStart;
  CLBuffer &modelRangeEnd;
  CLBuffer &isNonFullable;
  uint32_t rangeCount;
};

class LODManagerGPU
{
private:
  size_t localScanSize = 128;
  size_t groupCount;

  Scan fullScan;
  Scan nonFullScan;
  Scan impostorScan;
  Scan pointScan;

  Kernel &lodPassKernel;

  std::queue<cl_event> events;

  void initBuffers(Context &ctx, CommandQueue &queue, size_t totalObjects);
  void initKernels(LODGPUBuffers &data, LODSettings &settings, size_t totalObjects);

  template <typename Real>
  void updateKernels(const Camera &camera, FrameContext &ctx);

public:
  LODManagerGPU(ResourceManager &resourceManager);
  ~LODManagerGPU() = default;

  void init(Context &ctx, CommandQueue &queue, LODGPUBuffers &data, LODSettings &settings, size_t totalObjects);

  template <typename Real>
  void update(CommandQueue &queue, const Camera &camera, FrameContext &ctx, size_t totalObjects);

  CLBuffer &getIsFullBuffer() { return this->fullScan.getIsBuffer(); };
  CLBuffer &getIsNonFullBuffer() { return this->nonFullScan.getIsBuffer(); };
  CLBuffer &getIsImpostorBuffer() { return this->impostorScan.getIsBuffer(); };
  CLBuffer &getIsPointBuffer() { return this->pointScan.getIsBuffer(); };

  CLBuffer &getFullOffsetBuffer() { return this->fullScan.getOffsetBuffer(); };
  CLBuffer &getNonFullOffsetBuffer() { return this->nonFullScan.getOffsetBuffer(); };
  CLBuffer &getImpostorOffsetBuffer() { return this->impostorScan.getOffsetBuffer(); };
  CLBuffer &getPointOffsetBuffer() { return this->pointScan.getOffsetBuffer(); };
};

#include "render/lod/manager/lodManagerGPU.hpp"