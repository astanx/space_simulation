#include "physics/world/backend/physicsBackendGPU.h"

#include "debug/logger.h"

#include "physics/integrators/wisdomHolmanGPU.h"
#include "physics/integrators/eulerGPU.h"

#include "compute/context.h"

// Constructor
template <typename Real>
PhysicsBackendGPU<Real>::PhysicsBackendGPU(const PhysicsConfig &cfg, ResourceManager &manager, Context &ctx, AllIntegratorGPUBuffers &gpu, CommandQueue &queue, Total &total) : queue(queue), total(total)
{
  if (cfg.integrator == PhysicsIntegrator::WisdomHolman)
    this->integrator = std::make_unique<WisdomHolmanIntegratorGPU<Real>>(manager);
  else if (cfg.integrator == PhysicsIntegrator::Euler)
    this->integrator = std::make_unique<EulerIntegratorGPU<Real>>(manager);
  else if (cfg.integrator == PhysicsIntegrator::RK4)
    Logger::logFatal("Physics Backend GPU", "RK4 integrator is not implemented yet");
  else
    Logger::logFatal("Physics Backend CPU", "Unsupported physics integrator");

  this->integrator->init(gpu, this->total);
};

// Public functionss
template <typename Real>
void PhysicsBackendGPU<Real>::step(Real dt)
{
  this->integrator->step(this->queue, this->total, dt);
}