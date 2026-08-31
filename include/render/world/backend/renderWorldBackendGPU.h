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
  std::vector<size_t> specialIndices;
  std::vector<Range> specialAllocations;
  std::vector<glm::vec3> specialPositions;

  LODManagerGPU lodManagerGPU;
  RenderQueueBuilderGPU renderQueueBuilderGPU;

  CommandQueue &queue;
  Total &total;

  bool isDouble = false;
  bool wasSubInit = false;

  void initSpecialModel(RenderQueue &queue, InstanceManager &manager, const RenderDatabaseView &database, const Entity &entity);
  void initEntityQueue(RenderQueue &queue, InstanceManager &manager, const RenderDatabaseView &database, const Entity &entity) override;

  void updateSpecialPositions(CommandQueue &queue, InstanceManager &manager);

  size_t getSpecialIndex(const Entity &entity);

public:
  RenderWorldBackendGPU(ResourceManager &manager, CommandQueue &queue, Context &ctx, LODGPUBuffers &lodData, BackendGPUBuffers &data, Total &total, size_t modelCount);
  ~RenderWorldBackendGPU() = default;

  void sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light) override;
  void update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, FrameContext &ctx) override;
};