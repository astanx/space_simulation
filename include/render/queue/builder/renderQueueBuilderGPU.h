#pragma once

#include "compute/clBuffer.h"

#include <vector>
#include <queue>

class Kernel;
class ResourceManager;
class CommandQueue;
class Context;
class CommandQueue;
class InstanceManager;
class RenderQueue;
class Model;
class Camera;
class LODManagerGPU;
struct Total;
struct LODSettings;
struct FrameContext;
struct RenderQueueGPUData;

class RenderQueueBuilderGPU
{
private:
  std::queue<cl_event> events;
  Kernel &partitionObjectsKernel;

  CLBuffer modelFullCountBuffer;
  CLBuffer impostorCountBuffer;
  CLBuffer pointCountBuffer;

  void initKernels(RenderQueueGPUData &data, LODSettings &settings);
  void initBuffers(Context &ctx, CommandQueue &queue, size_t modelCount);

  void updateKernels(const Camera &camera, FrameContext &ctx);

public:
  RenderQueueBuilderGPU(ResourceManager &resourceManager);
  ~RenderQueueBuilderGPU() = default;

  void init(Context &ctx, CommandQueue &queue, RenderQueueGPUData &data, LODSettings &settings, size_t modelCount);

  void build(CommandQueue &commandQueue, RenderQueue &renderQueue, LODManagerGPU& lod, InstanceManager &instanceManager, const Camera &camera, FrameContext &ctx, std::vector<Model *> &models, size_t totalObjects);
};