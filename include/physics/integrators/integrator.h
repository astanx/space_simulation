#pragma once

#include <vector>

class Object;
class Integratable;

class Integrator
{
protected:
public:
  Integrator() = default;
  virtual ~Integrator() = default;

  virtual void step(std::vector<Integratable *> &objects, double dt) = 0;
};