#pragma once

#include "graphics/primitives/primitives.h"

#include "physics/structs/radii.h"

#include <iostream>

class AsteroidShape : public Primitive
{
private:
  Radii radii;
  double calculateR(double angle, double m, double a, double b, double n1, double n2, double n3);

public:
  AsteroidShape(double thetaSteps = 12, double phiSteps = 6, double m = 2.0, double a = 1.0, double b = 1.0, double n1 = 0.8, double n2 = 0.9, double n3 = 0.7, double scale = 1.0);

  const Radii &getRadii() const
  {
    return this->radii;
  }
};