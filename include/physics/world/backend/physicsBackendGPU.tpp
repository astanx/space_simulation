#include "physics/world/backend/physicsBackendGPU.h"

#include "debug/logger.h"

#include "physics/integrators/wisdomHolmanGPU.h"

#include "compute/context.h"

// Constructor
template <typename Real>
PhysicsBackendGPU<Real>::PhysicsBackendGPU(const PhysicsConfig &cfg, ResourceManager &manager, Context &ctx, IntegratorGPUBuffers &gpu, CommandQueue &queue, Total &total) : queue(queue), total(total)
{
  if (cfg.integrator == PhysicsIntegrator::WisdomHolman)
    this->integrator = std::make_unique<WisdomHolmanIntegratorGPU<Real>>(manager);
  else
    Logger::logFatal("Physics Backend CPU", "Unsupported physics integrator");

  this->integrator->init(gpu, this->total, ctx);
};

// Public functionss
template <typename Real>
void PhysicsBackendGPU<Real>::step(Real dt)
{
  this->integrator->step(this->queue, this->total, dt);
}