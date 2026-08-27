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
void PhysicsWorld<Real>::initCPUBackend(const EntityManager &entityManager, SharedDatabase<Real> &shared, ThreadPool &threadPool)
{
  IntegratorDatabase db{entityManager, shared, this->database};
  this->backend = std::make_unique<PhysicsBackendCPU<Real>>(db, threadPool);
}
template <typename Real>
void PhysicsWorld<Real>::initGPUBackend(ResourceManager &resourceManager, Context &ctx, CommandQueue &queue, IntegratorGPUBuffers &gpu, Total &total)
{
  this->backend = std::make_unique<PhysicsBackendGPU<Real>>(resourceManager, ctx, gpu, queue, total);
}

template <typename Real>
void PhysicsWorld<Real>::initGPUBuffers(Context &context)
{
  cl_context ctx = context.get();

  this->gpuBuffers.musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.mus.size() * sizeof(Real), this->database.mus.data());
  this->gpuBuffers.velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.velocities.size() * sizeof(Vec3<Real>), this->database.velocities.data());

  this->gpuBuffers.angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.angularVelocities.size() * sizeof(Vec3<Real>), this->database.angularVelocities.data());

  this->gpuBuffers.semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.semiAxises.size() * sizeof(Real), this->database.semiAxises.data());
  this->gpuBuffers.eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.eccentricities.size() * sizeof(Real), this->database.eccentricities.data());
  this->gpuBuffers.inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.inclinations.size() * sizeof(Real), this->database.inclinations.data());
  this->gpuBuffers.longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.longitude.size() * sizeof(Real), this->database.longitude.data());
  this->gpuBuffers.periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.periapsis.size() * sizeof(Real), this->database.periapsis.data());
  this->gpuBuffers.meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.meanAnomaly.size() * sizeof(Real), this->database.meanAnomaly.data());
  this->gpuBuffers.meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.meanMotion.size() * sizeof(Real), this->database.meanMotion.data());
  this->gpuBuffers.centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.centralBodyIndices.size() * sizeof(int), this->database.centralBodyIndices.data());

  this->gpuBuffers.quadrupoleTensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.quadrupoleTensors.size() * sizeof(Mat3<Real>), this->database.quadrupoleTensors.data());
  this->gpuBuffers.inertiaTensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.inertiaTensors.size() * sizeof(Mat3<Real>), this->database.inertiaTensors.data());
  this->gpuBuffers.loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.loveIndices.size() * sizeof(int), this->database.loveIndices.data());
  this->gpuBuffers.tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, this->database.tidalFactorIndices.size() * sizeof(int), this->database.tidalFactorIndices.data());
  this->gpuBuffers.loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.loveNumbers.size() * sizeof(Real), this->database.loveNumbers.data());
  this->gpuBuffers.tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, this->database.tidalFactors.size() * sizeof(Real), this->database.tidalFactors.data());
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