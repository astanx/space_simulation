#pragma once

#include "render/world/backend/renderWorldBackend.h"

#include "render/lod/manager/lodManagerGPU.h"
#include "render/queue/builder/renderQueueBuilderGPU.h"

#include <vector>

class Model;
class ResourceManager;
struct BackendGPUBuffers;

class RenderWorldBackendGPU : public RenderWorldBackend
{
private:
  std::vector<Model *> &models;

  std::vector<size_t> specialIndices;
  std::vector<Range> specialAllocations;
  std::vector<glm::vec3> specialPositions;

  LODManagerGPU lodManagerGPU;
  RenderQueueBuilderGPU renderQueueBuilderGPU;

  CommandQueue &queue;
  Total &total;

  bool isDouble;

  void initSpecialModel(RenderQueue &queue, InstanceManager &manager, const Model *model);
  void initModelQueue(RenderQueue &queue, InstanceManager &manager, const Model *model) override;

  void updateSpecialPositions(CommandQueue &queue, InstanceManager &manager);

  size_t getSpecialIndex(const Model *model);

public:
  RenderWorldBackendGPU(ResourceManager &manager, CommandQueue &queue, Context &ctx, LODGPUBuffers &lodData, BackendGPUBuffers &data, Total &total, std::vector<Model *> &models);
  ~RenderWorldBackendGPU() = default;

  void sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light) override;
  void update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, FrameContext &ctx) override;
};