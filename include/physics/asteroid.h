#pragma once

#include "physics/orbitalObject.h"
#include "render/modelSource.h"

class Model;

class Asteroid : public OrbitalObject
{
protected:
public:
  Asteroid(Object *centralBody, double mu, Radii radii, const KeplerElements &elements);
  ~Asteroid() = default;
};