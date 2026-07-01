#pragma once

#include "physics/structs/radii.h"

class RadiusSource
{
protected:
  Radii radii;

public:
  virtual ~RadiusSource() = default;
  double getRadius() const { return this->radii.mean; };
  Radii getRadii() const { return this->radii; };
};