#pragma once

#include "physics/object.h"

#include <vector>

class WisdomHolman
{
protected:
public:
  virtual void drift(double dt) = 0;
  virtual void halfKick(const std::vector<Object *> &bodies, double dt) = 0;
};