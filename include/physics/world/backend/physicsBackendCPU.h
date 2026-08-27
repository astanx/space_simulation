#pragma once

#include "physics/world/backend/physicsBackend.h"

#include "physics/integrators/data/integratorDatabase.h"

#include "physics/integrators/integratorCPU.h"

struct PhysicsCPUData;

template <typename Real>
class PhysicsBackendCPU : public PhysicsBackend<Real>
{
private:
  std::unique_ptr<IntegratorCPU<Real>> integrator;
  IntegratorDatabase<Real> database;

public:
  PhysicsBackendCPU(IntegratorDatabase<Real> &database, ThreadPool &threadPool);
  ~PhysicsBackendCPU() = default;

  void step(Real dt) override;
};

#include "physics/world/backend/physicsBackendCPU.tpp"