#pragma once

#include "physics/integrators/integrator.h"
#include "physics/integrators/data/integratorDatabase.h"
#include "physics/integrators/force/forceModel.h"

#include <vector>
#include <memory>

class Integratable;
class ThreadPool;

template <typename Real>
class IntegratorCPU : public Integrator
{
protected:
  ThreadPool &threadPool;
  std::unique_ptr<ForceModel<Real>> forceModel;

public:
  IntegratorCPU(ThreadPool &threadPool, std::unique_ptr<ForceModel<Real>> forceModel) : threadPool(threadPool), forceModel(std::move(forceModel)) {};
  virtual ~IntegratorCPU() = default;

  virtual void step(IntegratorDatabase<Real> &database, Real dt) = 0;
};