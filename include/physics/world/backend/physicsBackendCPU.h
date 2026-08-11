#pragma once

#include "physics/world/backend/physicsBackend.h"

#include "physics/integrators/integratorCPU.h"

struct PhysicsCPUData;

class PhysicsBackendCPU : public PhysicsBackend
{
private:
  PhysicsCPUData &data;
  std::unique_ptr<IntegratorCPU> integrator;

public:
  PhysicsBackendCPU(PhysicsCPUData &data);
  ~PhysicsBackendCPU() = default;

  void step(double dt) override;
};