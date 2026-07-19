#pragma once

#include "physics/integrators/integrator.h"

#include <vector>

struct PhysicsGPUData;
struct Total;
class Context;

class IntegratorGPU : public Integrator
{
protected:
public:
  IntegratorGPU() = default;
  ~IntegratorGPU() = default;

  virtual void init(PhysicsGPUData &gpu, Total &total, Context &ctx) = 0;
  virtual void step(Total &total, double dt) = 0;
};

template <typename Real>
class IIntegratorGPU : public IntegratorGPU
{
protected:
public:
  IIntegratorGPU() = default;
  ~IIntegratorGPU() = default;

  virtual void stepReal(Total &total, Real dt) = 0;
  void step(Total &total, double dt) override
  {
    this->stepReal(total, static_cast<Real>(dt));
  };
};