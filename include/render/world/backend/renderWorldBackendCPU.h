#pragma once

#include "render/world/backend/renderWorldBackend.h"

#include "render/lod/manager/lodManager.h"

#include <vector>

class ModelSource;
class RenderSystem;
class InstanceManager;

class RenderWorldBackendCPU : public RenderWorldBackend
{
private:
  std::vector<ModelSource *> &modelSources;
  std::vector<RenderSystem *> &renderSystems;
  std::vector<Model *> models;

  LODManager lodManager;

public:
  RenderWorldBackendCPU(InstanceManager& manager, std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems);
  ~RenderWorldBackendCPU() = default;

  void update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx) override;
};