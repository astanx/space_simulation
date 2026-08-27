#include "physics/world/backend/physicsBackendCPU.h"

#include "physics/world/data/physicsCPUData.h"

#include "physics/integrators/wisdomHolmanCPU.h"

#include <iostream>

// Constructor
template <typename Real>
PhysicsBackendCPU<Real>::PhysicsBackendCPU(IntegratorDatabase<Real> &database, ThreadPool &threadPool) : database(database)
{
  this->integrator = std::make_unique<WisdomHolmanIntegratorCPU<Real>>(threadPool);
};

// Public functions
template <typename Real>
void PhysicsBackendCPU<Real>::step(Real dt)
{
  this->integrator->step(this->database, dt);
}