#pragma once

#include "graphics/primitives/primitives.h"
#include "physics/structs/radii.h"

class Ellipsoid : public Primitive
{
public:
  Ellipsoid(unsigned segments, Radii radii);
};