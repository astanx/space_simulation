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
PhysicsWorld::PhysicsWorld() = default;
PhysicsWorld::~PhysicsWorld() = default;

// Public functions
void PhysicsWorld::initCPUBackend()
{
  this->backend = std::make_unique<PhysicsBackendCPU>(this->cpu);
}

void PhysicsWorld::initGPUBackend(ResourceManager &resourceManager, Context &ctx, CommandQueue &queue, IntegratorGPUBuffers &gpu, Total &total)
{
  this->backend = std::make_unique<PhysicsBackendGPU>(resourceManager, ctx, gpu, queue, total);
}

void PhysicsWorld::step(double dt)
{
  this->backend->step(dt);
}

void PhysicsWorld::addObject(Object *object)
{
  this->cpu.objects.push_back(object);
}
void PhysicsWorld::addAtmosphere(Atmosphere *atmosphere)
{
  this->cpu.atmospheres.push_back(atmosphere);
}
void PhysicsWorld::addPlanetarObject(std::unique_ptr<Planet> planetarObject)
{
  this->cpu.planetarObjectViews.push_back(planetarObject.get());
  this->cpu.planetarObjects.push_back(std::move(planetarObject));
}
void PhysicsWorld::addIntegratable(Integratable *object)
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
void PhysicsWorld::addSun(Star *sun)
{
  if (this->sun)
    Logger::logFatal("PhysicsWorld", "Sun already exists");

  this->sun = sun;
}
void PhysicsWorld::addStar(std::unique_ptr<Star> star)
{
  this->cpu.stars.push_back(std::move(star));
}
void PhysicsWorld::addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem)
{
  this->cpu.asteroidSystemViews.push_back(asteroidSystem.get());
  this->cpu.asteroidSystems.push_back(std::move(asteroidSystem));
}
void PhysicsWorld::addSystem(System *system)
{
  this->cpu.systems.push_back(system);
}
const Star &PhysicsWorld::getSun() const
{
  if (!this->sun)
    Logger::logFatal("PhysicsWorld", "No sun");

  return *this->sun;
};
const std::vector<AsteroidSystem *> &PhysicsWorld::getAsteroidSystems() const
{
  if (this->cpu.asteroidSystemViews.empty())
    Logger::logWarning("PhysicsWorld", "Asteroid systems are empty");

  return this->cpu.asteroidSystemViews;
};
const std::vector<Planet *> &PhysicsWorld::getPlanetarObjects() const
{
  if (this->cpu.planetarObjectViews.empty())
    Logger::logWarning("PhysicsWorld", "Planets are empty");

  return this->cpu.planetarObjectViews;
};

const std::vector<Object *> &PhysicsWorld::getObjects() const
{
  return this->cpu.objects;
};

const std::vector<System *> &PhysicsWorld::getSystems() const
{
  return this->cpu.systems;
};