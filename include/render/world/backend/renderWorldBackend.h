#pragma once

#include "render/lod/lodSettings.h"

#include <vector>

class Camera;
class RenderQueue;
class FrameContext;
class InstanceManager;
class Model;

class RenderWorldBackend
{
protected:
  LODSettings lodSettings;

  bool subQueuesInitialized = false;

  void initSubQueues(RenderQueue &queue, InstanceManager &manager, std::vector<Model *> &models);

public:
  RenderWorldBackend() = default;
  virtual ~RenderWorldBackend() = default;

  virtual void update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx) = 0;
};