#pragma once

#include "physics/world/backend/physicsBackend.h"

#include "physics/world/physicsConfig.h"

#include "physics/integrators/integratorGPU.h"

class ResourceManager;
class CommandQueue;
class Context;
struct Total;
struct AllIntegratorGPUBuffers;

template <typename Real>
class PhysicsBackendGPU : public PhysicsBackend<Real>
{
private:
  CommandQueue &queue;
  Total &total;

  std::unique_ptr<IntegratorGPU> integrator;

public:
  PhysicsBackendGPU(const PhysicsConfig &cfg, ResourceManager &manager, Context &ctx, AllIntegratorGPUBuffers &data, CommandQueue &queue, Total &total);
  ~PhysicsBackendGPU() = default;

  void step(Real dt) override;
};

#include "physics/world/backend/physicsBackendGPU.tpp"