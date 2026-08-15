#pragma once

#include "render/world/backend/renderWorldBackend.h"

#include "render/lod/manager/lodManagerGPU.h"
#include "render/queue/builder/renderQueueBuilderGPU.h"

#include <vector>

class Model;
class ResourceManager;
struct BackendGPUData;

class RenderWorldBackendGPU : public RenderWorldBackend
{
private:
  std::vector<Model *> &models;

  LODManagerGPU lodManagerGPU;
  RenderQueueBuilderGPU renderQueueBuilderGPU;

  CommandQueue &queue;
  Total &total;

  bool isDouble;

public:
  RenderWorldBackendGPU(ResourceManager &manager, CommandQueue &queue, Context &ctx, LODGPUData &lodData, BackendGPUData &data, Total &total, std::vector<Model *> &models);
  ~RenderWorldBackendGPU() = default;

  void sync(PhysicsWorld &physics, PointLight* light) override;
  void update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx) override;
};