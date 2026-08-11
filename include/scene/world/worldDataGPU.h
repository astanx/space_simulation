#pragma once

#include "scene/world/data/sharedDataGPU.h"
#include "physics/world/data/physicsDataGPU.h"
#include "render/world/data/renderDataGPU.h"

#include "physics/world/total.h"

template <typename Real>
struct WorldDataGPU
{
  SharedDataGPU<Real> shared;
  PhysicsDataGPU<Real> physics;
  RenderDataGPU render;

  void resize(size_t n)
  {
    this->shared.resize(n);
    this->physics.resize(n);
    this->render.resize(n);
  }

  void combine(WorldDataGPU &data)
  {
    this->shared.combine(data.shared);
    this->physics.combine(data.physics);
    this->render.combine(data.render);
  }
};
