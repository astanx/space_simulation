#include "physics/world/backend/physicsBackendCPU.h"

#include "physics/world/data/physicsCPUData.h"

#include "physics/integrators/wisdomHolmanCPU.h"

#include <iostream>

// Constructor
PhysicsBackendCPU::PhysicsBackendCPU(PhysicsCPUData &data) : data(data)
{
  this->integrator = std::make_unique<WisdomHolmanIntegratorCPU>();
};

// Public functions
void PhysicsBackendCPU::step(double dt)
{
  this->integrator->step(data.integratableObjects, data.integratableSystems, dt);
}