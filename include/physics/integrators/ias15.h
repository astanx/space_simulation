#pragma once

#include "physics/integrators/integrator.h"

#include <vector>

class IAS15Integrator : public Integrator
{
protected:
public:
  IAS15Integrator() = default;
  ~IAS15Integrator() = default;

  void step(std::vector<Integratable *> &objects, double dt) = 0;
};