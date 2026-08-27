#include "physics/world/backend/physicsBackendGPU.h"

#include "physics/integrators/wisdomHolmanGPU.h"

#include "compute/context.h"

// Constructor
template <typename Real>
PhysicsBackendGPU<Real>::PhysicsBackendGPU(ResourceManager &manager, Context &ctx, IntegratorGPUBuffers &gpu, CommandQueue &queue, Total &total) : queue(queue), total(total)
{
  this->integrator = std::make_unique<WisdomHolmanIntegratorGPU<Real>>(manager);
  this->integrator->init(gpu, this->total, ctx);
};

// Public functionss
template <typename Real>
void PhysicsBackendGPU<Real>::step(Real dt)
{
  this->integrator->step(this->queue, this->total, dt);
}