#pragma once

#include "scene/frameContext.h"

struct RenderSettings
{
  bool paused;
  bool useBloom;
  bool useHDR;
  double exposure;
};

struct RenderContext
{
  RenderSettings settings;
  FrameContext frameCtx;
  float deltaTime;
};