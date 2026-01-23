#pragma once

#include "graphics/primitives/primitives.h"

class AsteroidShape : public Primitive
{
private:
  double calculateR(double angle, double m, double a, double b, double n1, double n2, double n3);
public:
  AsteroidShape(double thetaSteps = 12, double phiSteps = 6, double m = 2.0, double a = 1.0, double b = 1.0, double n1 = 0.8, double n2 = 0.9, double n3 = 0.7);
};