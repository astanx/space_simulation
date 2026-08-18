#include "physics/world/backend/physicsBackendGPU.h"

#include "physics/integrators/wisdomHolmanGPU.h"

#include "compute/context.h"

// Constructor
PhysicsBackendGPU::PhysicsBackendGPU(ResourceManager &manager, Context &ctx, IntegratorGPUBuffers &gpu, CommandQueue &queue, Total &total) : queue(queue), total(total)
{
  if (ctx.getSupportsDouble())
    this->integrator = std::make_unique<WisdomHolmanIntegratorGPU<double>>(manager);
  else
    this->integrator = std::make_unique<WisdomHolmanIntegratorGPU<float>>(manager);

  this->integrator->init(gpu, this->total, ctx);
};

// Public functions
void PhysicsBackendGPU::step(double dt)
{
  this->integrator->step(this->queue, this->total, dt);
}