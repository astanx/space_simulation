#pragma once

#include "scene/world/data/sharedDatabase.h"
#include "physics/world/data/physicsDatabase.h"
#include "render/world/data/renderDatabase.h"

template <typename Real>
struct WorldDatabase
{
  SharedDatabase<Real> shared;
  PhysicsDatabase<Real> physics;
  RenderDatabase render;

  void resize(size_t n)
  {
    this->shared.resize(n);
    this->physics.resize(n);
    this->render.resize(n);
  }

  void combine(WorldDatabase &data)
  {
    this->shared.combine(data.shared);
    this->physics.combine(data.physics);
    this->render.combine(data.render);
  }
};
