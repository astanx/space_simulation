#include "physics/world/backend/physicsBackendCPU.h"

#include "debug/logger.h"

#include "physics/world/data/physicsCPUData.h"

#include "physics/integrators/wisdomHolmanCPU.h"

#include "physics/integrators/force/directForceModel.h"

#include <iostream>

// Constructor
template <typename Real>
PhysicsBackendCPU<Real>::PhysicsBackendCPU(const PhysicsConfig &cfg, IntegratorDatabase<Real> &database, ThreadPool &threadPool) : database(database)
{
  std::unique_ptr<ForceModel<Real>> forceModel;
  if (cfg.forceModel == PhysicsForceModel::Direct)
    forceModel = std::make_unique<DirectForceModel<Real>>(threadPool);
  else
    Logger::logFatal("Physics Backend CPU", "Unsupported force model");

  if (cfg.integrator == PhysicsIntegrator::WisdomHolman)
    this->integrator = std::make_unique<WisdomHolmanIntegratorCPU<Real>>(threadPool, std::move(forceModel));
  else
    Logger::logFatal("Physics Backend CPU", "Unsupported physics integrator");
};

// Public functions
template <typename Real>
void PhysicsBackendCPU<Real>::step(Real dt)
{
  this->integrator->step(this->database, dt);
}