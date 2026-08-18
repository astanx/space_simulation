#pragma once

#include "physics/world/backend/physicsBackend.h"

#include "physics/integrators/integratorGPU.h"

class ResourceManager;
class CommandQueue;
class Context;
struct Total;
struct IntegratorGPUBuffers;

class PhysicsBackendGPU : public PhysicsBackend
{
private:
  CommandQueue &queue;
  Total &total;

  std::unique_ptr<IntegratorGPU> integrator;

public:
  PhysicsBackendGPU(ResourceManager &manager, Context &ctx, IntegratorGPUBuffers &data, CommandQueue &queue, Total &total);
  ~PhysicsBackendGPU() = default;

  void step(double dt) override;
};