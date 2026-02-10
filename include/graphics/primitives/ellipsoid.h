#pragma once

#include "graphics/primitives/primitives.h"

struct Radii;

class Ellipsoid : public Primitive
{
public:
  Ellipsoid(unsigned segments, Radii radii);
};