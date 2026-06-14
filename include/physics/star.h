#pragma once

#include "physics/object.h"

#include "render/modelSource.h"

class Model;

class Star : public Object, public ModelSource
{
protected:
  double luminosity;

public:
  Star(double mu, Radii radii, double luminosity, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0), TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField());
  ~Star() = default;

  double getLuminosity() const { return this->luminosity; }
};