#pragma once

#include "scene/frameContext.h"

struct RenderSettings
{
  bool paused;
  bool useBloom;
  bool useHDR;
  double exposure;
  double bloomPower;
};

struct RenderContext
{
  RenderSettings settings;
  FrameContext frameCtx;
  float deltaTime;
};