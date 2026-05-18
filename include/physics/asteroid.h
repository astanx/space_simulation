#pragma once

#include "physics/orbitalObject.h"
#include "render/modelSource.h"

class Model;

class Asteroid : public OrbitalObject, public ModelSource
{
protected:
public:
  Asteroid(Object *centralBody, double mu, double radius, const KeplerElements &elements);
  ~Asteroid() = default;

  void renderInstanced();

  glm::dmat4 getModelMatrix();
};