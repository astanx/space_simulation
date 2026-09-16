#pragma once

#include "physics/world/physicsConfig.h"

#include "render/world/renderConfig.h"

#include "resources/data/precision.h"

struct WorldConfig
{
  PhysicsConfig physics;
  RenderConfig render;

  Precision precision = Precision::DOUBLE; // --precision <float|double>
  double startTime;
};