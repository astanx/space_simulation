#pragma once

#include "render/lod/lodSettings.h"

class Camera;
class RenderQueue;
class FrameContext;
class InstanceManager;

class RenderWorldBackend
{
protected:
  LODSettings lodSettings;

public:
  RenderWorldBackend() = default;
  virtual ~RenderWorldBackend() = default;

  virtual void update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx) = 0;
};