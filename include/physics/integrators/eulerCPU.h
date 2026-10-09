#pragma once

#include "physics/integrators/integratorCPU.h"

#include "resources/data/realTypes.h"

class Object;
class OrbitalObject;
class Integratable;

template <typename Real>
class EulerIntegratorCPU : public IntegratorCPU<Real>
{
protected:
  using vec3 = typename RealTypes<Real>::vec3;
  using mat3 = typename RealTypes<Real>::mat3;
  using quat = typename RealTypes<Real>::quat;

  void driftLinear(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt);
  void driftAngular(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt);

  void drift(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt);

public:
  EulerIntegratorCPU(ThreadPool &threadPool, std::unique_ptr<ForceModel<Real>> forceModel) : IntegratorCPU<Real>(threadPool, std::move(forceModel)) {};
  ~EulerIntegratorCPU() = default;

  void step(IntegratorDatabase<Real> &database, Real dt) override;
};

#include "physics/integrators/eulerCPU.hpp"