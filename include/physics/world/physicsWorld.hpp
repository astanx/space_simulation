#pragma once

#include "physics/world/physicsWorld.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/planet.h"
#include "physics/star.h"
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
PhysicsWorld<Real>::PhysicsWorld() : sun(nullptr) {};
template <typename Real>
PhysicsWorld<Real>::~PhysicsWorld() = default;

// Public functions
template <typename Real>
void PhysicsWorld<Real>::initCPUBackend()
{
  this->backend = std::make_unique<PhysicsBackendCPU>(this->cpu);
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

  this->gpu.musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.mus.size() * sizeof(Real), data.mus.data());
  this->gpu.velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.velocities.size() * sizeof(Vec3<Real>), data.velocities.data());

  this->gpu.angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.angularVelocities.size() * sizeof(Vec3<Real>), data.angularVelocities.data());

  this->gpu.semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.semiAxises.size() * sizeof(Real), data.semiAxises.data());
  this->gpu.eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.eccentricities.size() * sizeof(Real), data.eccentricities.data());
  this->gpu.inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.inclinations.size() * sizeof(Real), data.inclinations.data());
  this->gpu.longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.longitude.size() * sizeof(Real), data.longitude.data());
  this->gpu.periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.periapsis.size() * sizeof(Real), data.periapsis.data());
  this->gpu.meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.meanAnomaly.size() * sizeof(Real), data.meanAnomaly.data());
  this->gpu.meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.meanMotion.size() * sizeof(Real), data.meanMotion.data());
  this->gpu.centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.centralBodyIndices.size() * sizeof(int), data.centralBodyIndices.data());

  this->gpu.quadrupoleTensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.quadrupoleTensors.size() * sizeof(Mat3<Real>), data.quadrupoleTensors.data());
  this->gpu.inertiaTensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.inertiaTensors.size() * sizeof(Mat3<Real>), data.inertiaTensors.data());
  this->gpu.loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.loveIndices.size() * sizeof(int), data.loveIndices.data());
  this->gpu.tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.tidalFactorIndices.size() * sizeof(int), data.tidalFactorIndices.data());
  this->gpu.loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.loveNumbers.size() * sizeof(Real), data.loveNumbers.data());
  this->gpu.tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.tidalFactors.size() * sizeof(Real), data.tidalFactors.data());
}

template <typename Real>
void PhysicsWorld<Real>::step(double dt)
{
  this->backend->step(dt);
}
template <typename Real>
void PhysicsWorld<Real>::addObject(Object *object)
{
  this->cpu.objects.push_back(object);
}
template <typename Real>
void PhysicsWorld<Real>::addAtmosphere(Atmosphere *atmosphere)
{
  this->cpu.atmospheres.push_back(atmosphere);
}
template <typename Real>
void PhysicsWorld<Real>::addPlanetarObject(std::unique_ptr<Planet> planetarObject)
{
  this->cpu.planetarObjectViews.push_back(planetarObject.get());
  this->cpu.planetarObjects.push_back(std::move(planetarObject));
}
template <typename Real>
void PhysicsWorld<Real>::addIntegratable(Integratable *object)
{
  std::vector<Object *> integratableObjects;
  std::vector<System *> integratableSystems;

  Object *obj = dynamic_cast<Object *>(object);
  if (obj)
  {
    this->cpu.integratableObjects.push_back(obj);
    return;
  }

  System *sys = dynamic_cast<System *>(object);
  if (sys)
  {
    this->cpu.integratableSystems.push_back(sys);
    return;
  }

  this->cpu.integratable.push_back(object);
}
template <typename Real>
void PhysicsWorld<Real>::addSun(Star *sun)
{
  if (this->sun)
    Logger::logFatal("PhysicsWorld", "Sun already exists");

  this->sun = sun;
}
template <typename Real>
void PhysicsWorld<Real>::addStar(std::unique_ptr<Star> star)
{
  this->cpu.stars.push_back(std::move(star));
}
template <typename Real>
void PhysicsWorld<Real>::addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem)
{
  this->cpu.asteroidSystemViews.push_back(asteroidSystem.get());
  this->cpu.asteroidSystems.push_back(std::move(asteroidSystem));
}
template <typename Real>
void PhysicsWorld<Real>::addSystem(System *system)
{
  this->cpu.systems.push_back(system);
}
template <typename Real>
const Star &PhysicsWorld<Real>::getSun() const
{
  if (!this->sun)
    Logger::logFatal("PhysicsWorld", "No sun");

  return *this->sun;
};
template <typename Real>
const std::vector<AsteroidSystem *> &PhysicsWorld<Real>::getAsteroidSystems() const
{
  if (this->cpu.asteroidSystemViews.empty())
    Logger::logWarning("PhysicsWorld", "Asteroid systems are empty");

  return this->cpu.asteroidSystemViews;
};
template <typename Real>
const std::vector<Planet *> &PhysicsWorld<Real>::getPlanetarObjects() const
{
  if (this->cpu.planetarObjectViews.empty())
    Logger::logWarning("PhysicsWorld", "Planets are empty");

  return this->cpu.planetarObjectViews;
};
template <typename Real>
const std::vector<Object *> &PhysicsWorld<Real>::getObjects() const
{
  return this->cpu.objects;
};
template <typename Real>
const std::vector<System *> &PhysicsWorld<Real>::getSystems() const
{
  return this->cpu.systems;
};
