#pragma once

#include "graphics/primitives/primitives.h"

struct Radii;

class Ellipsoid : public Primitive
{
protected:
public:
  Ellipsoid(unsigned segments, Radii radii, bool tangent = false);
};