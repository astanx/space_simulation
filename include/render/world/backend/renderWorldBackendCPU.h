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
  LODManager lodManager;

public:
  RenderWorldBackendCPU();
  ~RenderWorldBackendCPU() = default;

  void sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light) override;
  void update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, TrailManager &trailManager, RenderContext &ctx) override;
};