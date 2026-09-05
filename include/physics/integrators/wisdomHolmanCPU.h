#pragma once

#include "physics/integrators/integratorCPU.h"

class Object;
class OrbitalObject;
class Integratable;

template <typename Real>
class WisdomHolmanIntegratorCPU : public IntegratorCPU<Real>
{
protected:
  using vec3 = typename IntegratorTypes<Real>::vec3;
  using mat3 = typename IntegratorTypes<Real>::mat3;
  using quat = typename IntegratorTypes<Real>::quat;

  bool validEntity(const std::unique_ptr<Entity> &entity);

  void halfKickLinear(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt);
  void driftLinear(const Entity &entity, IntegratorDatabase<Real> &database, Real dt);

  void halfKickAngular(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt);
  void driftAngular(const Entity &entity, IntegratorDatabase<Real> &database, Real dt);

  void halfKick(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt);
  void drift(const Entity &entity, IntegratorDatabase<Real> &database, Real dt);

  void keplerDrift(const Entity &entity, IntegratorDatabase<Real> &database, Real dt);

public:
  WisdomHolmanIntegratorCPU(ThreadPool &threadPool) : IntegratorCPU<Real>(threadPool) {};
  ~WisdomHolmanIntegratorCPU() = default;

  void step(IntegratorDatabase<Real> &database, Real dt) override;
};

#include "physics/integrators/wisdomHolmanCPU.tpp"