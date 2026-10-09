#pragma once

#include "physics/integrators/integrator.h"

#include <vector>

struct AllIntegratorGPUBuffers;
struct Total;
class CommandQueue;
class Context;

class IntegratorGPU : public Integrator
{
protected:
public:
  IntegratorGPU() = default;
  ~IntegratorGPU() = default;

  virtual void init(AllIntegratorGPUBuffers &gpu, Total &total) = 0;
  virtual void step(CommandQueue& queue, Total &total, double dt) = 0;
};

template <typename Real>
class IIntegratorGPU : public IntegratorGPU
{
protected:
public:
  IIntegratorGPU() = default;
  ~IIntegratorGPU() = default;

  virtual void stepReal(CommandQueue& queue, Total &total, Real dt) = 0;
  void step(CommandQueue& queue, Total &total, double dt) override
  {
    this->stepReal(queue, total, static_cast<Real>(dt));
  };
};