#pragma once

#include "maths/momentsMaths.h"

struct Radii;

struct InertiaProperties
{
  double A;
  double B;
  double C;

  InertiaProperties(double mass, Radii radii)
  {
    this->A = MomentsMaths::calculateA(mass, radii);
    this->B = MomentsMaths::calculateB(mass, radii);
    this->C = MomentsMaths::calculateC(mass, radii);
  }
};