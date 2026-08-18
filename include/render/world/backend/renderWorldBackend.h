#pragma once

#include "render/lod/lodSettings.h"

#include <vector>
#include <glm/glm.hpp>

class Camera;
class RenderQueue;
class FrameContext;
class InstanceManager;
class Model;
class PhysicsWorld;
class PointLight;
struct Range;

class RenderWorldBackend
{
protected:
  LODSettings lodSettings;

  bool subQueuesInitialized = false;

  void initShadowQueue(RenderQueue &queue, InstanceManager &manager, Model *model);
  void initReflectorQueue(RenderQueue &queue, InstanceManager &manager, Model *model);
  virtual void initModelQueue(RenderQueue &queue, InstanceManager &manager, Model *model);
  void initSubQueues(RenderQueue &queue, InstanceManager &manager, std::vector<Model *> &models);

  void moveSunLight(glm::vec3 position, PointLight *light);

public:
  RenderWorldBackend() = default;
  virtual ~RenderWorldBackend() = default;

  virtual void sync(PhysicsWorld &physics, PointLight *light) = 0;
  virtual void update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx) = 0;
};