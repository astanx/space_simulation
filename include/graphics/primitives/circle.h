#pragma once

#include "graphics/primitives/primitives.h"

class Circle : public Primitive
{
public:
  Circle(unsigned segments, float radius);
};