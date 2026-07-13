#pragma once

#include "physics/integrators/integrator.h"

#include <vector>

class Integratable;
class Context;

class IntegratorGPU : public Integrator
{
protected:
public:
  IntegratorGPU() = default;
  ~IntegratorGPU() = default;

  virtual void init(std::vector<Integratable *> &objects, Context& ctx) = 0;
  virtual void step(double dt) = 0;
};

template <typename Real>
class IIntegratorGPU : public IntegratorGPU
{
protected:
public:
  IIntegratorGPU() = default;
  ~IIntegratorGPU() = default;

  virtual void stepReal(Real dt) = 0;
  void step(double dt) override
  {
    this->stepReal(static_cast<Real>(dt));
  };
};