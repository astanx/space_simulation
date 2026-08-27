#pragma once

#include "physics/integrators/integrator.h"
#include "physics/integrators/data/integratorDatabase.h"

#include <vector>

class Integratable;
class ThreadPool;

template <typename Real>
class IntegratorCPU : public Integrator
{
protected:
  ThreadPool &threadPool;

public:
  IntegratorCPU(ThreadPool &threadPool) : threadPool(threadPool) {};
  virtual ~IntegratorCPU() = default;

  virtual void step(IntegratorDatabase<Real> &database, Real dt) = 0;
};