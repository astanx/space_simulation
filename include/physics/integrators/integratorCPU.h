#pragma once

#include "physics/integrators/integrator.h"
#include "physics/integrators/data/integratorDatabase.h"

#include <vector>

class Integratable;
class ThreadPool;

template <typename Real>
struct IntegratorTypes
{
  using vec3 = glm::vec<3, Real>;
  using mat3 = glm::mat<3, 3, Real>;
  using quat = glm::qua<Real>;
};

template <typename Real>
class IntegratorCPU : public Integrator
{
protected:
  using vec3 = typename IntegratorTypes<Real>::vec3;
  using mat3 = typename IntegratorTypes<Real>::mat3;
  using quat = typename IntegratorTypes<Real>::quat;

  ThreadPool &threadPool;

  Real calculateEnergy(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database);

public:
  IntegratorCPU(ThreadPool &threadPool) : threadPool(threadPool) {};
  virtual ~IntegratorCPU() = default;

  virtual void step(IntegratorDatabase<Real> &database, Real dt) = 0;
};

#include "physics/integrators/integratorCPU.hpp"