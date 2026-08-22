#pragma once

#include "physics/world/backend/physicsBackend.h"

#include "physics/integrators/integratorCPU.h"

struct PhysicsCPUData;

class PhysicsBackendCPU : public PhysicsBackend
{
private:
  std::unique_ptr<IntegratorCPU> integrator;

public:
  PhysicsBackendCPU();
  ~PhysicsBackendCPU() = default;

  void step(double dt) override;
};