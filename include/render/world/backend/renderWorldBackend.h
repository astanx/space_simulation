#pragma once

#include "render/lod/lodSettings.h"

#include <vector>
#include <glm/glm.hpp>

class Camera;
class RenderQueue;
class FrameContext;
class InstanceManager;
class Model;
class IPhysicsWorld;
class PointLight;
struct Range;
struct RenderDatabaseView;

class RenderWorldBackend
{
protected:
  LODSettings lodSettings;

  size_t lastModelsSize = 0;

  void initShadowQueue(RenderQueue &queue, InstanceManager &manager, const Model *model);
  void initReflectorQueue(RenderQueue &queue, InstanceManager &manager, const Model *model);
  virtual void initModelQueue(RenderQueue &queue, InstanceManager &manager, const Model *model);
  void initSubQueues(RenderQueue &queue, InstanceManager &manager, const std::vector<Model *> &models);

public:
  RenderWorldBackend() = default;
  virtual ~RenderWorldBackend() = default;

  virtual void sync(IPhysicsWorld &physics, const RenderDatabaseView& database, PointLight *light) = 0;
  virtual void update(RenderQueue &queue, const RenderDatabaseView& database, InstanceManager &instanceManager, FrameContext &ctx)  = 0;
};