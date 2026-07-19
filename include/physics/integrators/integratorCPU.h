#pragma once

#include "physics/integrators/integrator.h"

#include <vector>

class Integratable;

class IntegratorCPU : public Integrator
{
protected:
public:
  IntegratorCPU() = default;
  virtual ~IntegratorCPU() = default;

  virtual void step(std::vector<Object *> &objects, std::vector<System*> &systems, double dt) = 0;
};