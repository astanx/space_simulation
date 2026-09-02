#pragma once

#include "render/lod/lodSettings.h"

#include <vector>
#include <glm/glm.hpp>

class Camera;
class RenderQueue;
class InstanceManager;
class Model;
class IPhysicsWorld;
class PointLight;
class TrailManager;
struct RenderContext;
struct Range;
struct RenderDatabaseView;
struct Entity;

class RenderWorldBackend
{
protected:
  LODSettings lodSettings;

  size_t lastEntityCount = 0;

  void initShadowQueue(RenderQueue &queue, InstanceManager &manager, const Model *model);
  void initReflectorQueue(RenderQueue &queue, InstanceManager &manager, const Model *model);
  virtual void initEntityQueue(RenderQueue &queue, InstanceManager &manager, const RenderDatabaseView &database, const Entity entity);
  void initSubQueues(RenderQueue &queue, InstanceManager &manager, const RenderDatabaseView &database);

  void updateSpecialModel(Model *model, const glm::vec3 &position);

public:
  RenderWorldBackend() = default;
  virtual ~RenderWorldBackend() = default;

  virtual void sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light) = 0;
  virtual void update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, TrailManager &trailManager, RenderContext &ctx) = 0;
};