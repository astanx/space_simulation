#pragma once

#include "physics/world/physicsWorld.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/planet.h"
#include "physics/systems/asteroidSystem.h"

#include "physics/integrators/integratorCPU.h"
#include "physics/integrators/integratorGPU.h"

#include "physics/integrators/wisdomHolmanCPU.h"
#include "physics/integrators/wisdomHolmanGPU.h"

#include "physics/world/backend/physicsBackendCPU.h"
#include "physics/world/backend/physicsBackendGPU.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "debug/logger.h"

// Constructor / Destructor
template <typename Real>
PhysicsWorld<Real>::PhysicsWorld() = default;
template <typename Real>
PhysicsWorld<Real>::~PhysicsWorld() = default;

// Public functions
template <typename Real>
void PhysicsWorld<Real>::initCPUBackend()
{
  this->backend = std::make_unique<PhysicsBackendCPU>();
}
template <typename Real>
void PhysicsWorld<Real>::initGPUBackend(ResourceManager &resourceManager, Context &ctx, CommandQueue &queue, IntegratorGPUBuffers &gpu, Total &total)
{
  this->backend = std::make_unique<PhysicsBackendGPU>(resourceManager, ctx, gpu, queue, total);
}

template <typename Real>
void PhysicsWorld<Real>::initGPUBuffers(Context &context, PhysicsDatabase<Real> data)
{
  cl_context ctx = context.get();

  this->gpuBuffers.musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.mus.size() * sizeof(Real), data.mus.data());
  this->gpuBuffers.velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.velocities.size() * sizeof(Vec3<Real>), data.velocities.data());

  this->gpuBuffers.angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.angularVelocities.size() * sizeof(Vec3<Real>), data.angularVelocities.data());

  this->gpuBuffers.semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.semiAxises.size() * sizeof(Real), data.semiAxises.data());
  this->gpuBuffers.eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.eccentricities.size() * sizeof(Real), data.eccentricities.data());
  this->gpuBuffers.inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.inclinations.size() * sizeof(Real), data.inclinations.data());
  this->gpuBuffers.longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.longitude.size() * sizeof(Real), data.longitude.data());
  this->gpuBuffers.periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.periapsis.size() * sizeof(Real), data.periapsis.data());
  this->gpuBuffers.meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.meanAnomaly.size() * sizeof(Real), data.meanAnomaly.data());
  this->gpuBuffers.meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.meanMotion.size() * sizeof(Real), data.meanMotion.data());
  this->gpuBuffers.centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.centralBodyIndices.size() * sizeof(int), data.centralBodyIndices.data());

  this->gpuBuffers.quadrupoleTensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.quadrupoleTensors.size() * sizeof(Mat3<Real>), data.quadrupoleTensors.data());
  this->gpuBuffers.inertiaTensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.inertiaTensors.size() * sizeof(Mat3<Real>), data.inertiaTensors.data());
  this->gpuBuffers.loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.loveIndices.size() * sizeof(int), data.loveIndices.data());
  this->gpuBuffers.tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.tidalFactorIndices.size() * sizeof(int), data.tidalFactorIndices.data());
  this->gpuBuffers.loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.loveNumbers.size() * sizeof(Real), data.loveNumbers.data());
  this->gpuBuffers.tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.tidalFactors.size() * sizeof(Real), data.tidalFactors.data());
}

template <typename Real>
void PhysicsWorld<Real>::step(double dt)
{
  this->backend->step(dt);
}
template <typename Real>
void PhysicsWorld<Real>::addSun(const Entity &sun)
{
  if (this->sun)
    Logger::logFatal("Physics World", "Sun already exists");

  this->sun = &sun;
}
template <typename Real>

void PhysicsWorld<Real>::setDatabase(PhysicsDatabase<Real> database)
{
  if (this->wasDatabaseInit)
    Logger::logWarning("Physics World", "Database was already initialized");

  this->database = database;
  this->wasDatabaseInit = true;
}

template <typename Real>
const Entity &PhysicsWorld<Real>::getSun() const
{
  if (!this->sun)
    Logger::logFatal("PhysicsWorld", "No sun");

  return *this->sun;
};