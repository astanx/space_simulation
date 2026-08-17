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

  std::vector<size_t> specialIndices;
  std::vector<Range> specialAllocations;
  std::vector<glm::vec3> specialPositions;

  LODManagerGPU lodManagerGPU;
  RenderQueueBuilderGPU renderQueueBuilderGPU;

  CommandQueue &queue;
  Total &total;

  bool isDouble;

  void initSpecialModel(RenderQueue &queue, InstanceManager &manager, Model *model);
  void initModelQueue(RenderQueue &queue, InstanceManager &manager, Model *model) override;

  void updateSpecialPositions(CommandQueue &queue, InstanceManager &manager);

  size_t getSpecialIndex(Model* model);

public:
  RenderWorldBackendGPU(ResourceManager &manager, CommandQueue &queue, Context &ctx, LODGPUData &lodData, BackendGPUData &data, Total &total, std::vector<Model *> &models);
  ~RenderWorldBackendGPU() = default;

  void sync(PhysicsWorld &physics, PointLight *light) override;
  void update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx) override;
};