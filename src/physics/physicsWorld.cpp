#include "physics/physicsWorld.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/planet.h"
#include "physics/star.h"
#include "physics/systems/asteroidSystem.h"

#include "physics/integrators/integratorCPU.h"
#include "physics/integrators/integratorGPU.h"

#include "physics/integrators/wisdomHolmanCPU.h"
#include "physics/integrators/wisdomHolmanGPU.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "debug/logger.h"

// Constructor / Destructor
PhysicsWorld::PhysicsWorld()
{
  this->integratorCPU = std::make_unique<WisdomHolmanIntegratorCPU>();
}
PhysicsWorld::~PhysicsWorld() = default;

// Public functions
void PhysicsWorld::initGPU(ResourceManager &resourceManager, Context &ctx)
{
  if (ctx.getSupportsDouble())
    this->integratorGPU = std::make_unique<WisdomHolmanIntegratorGPU<double>>(resourceManager);
  else
    this->integratorGPU = std::make_unique<WisdomHolmanIntegratorGPU<float>>(resourceManager);

  this->integratorGPU->init(this->integratableObjects, ctx);
}
void PhysicsWorld::step(double dt)
{
  // this->integratorCPU->step(this->integratableObjects, dt);
  this->integratorGPU->step(dt);
}

void PhysicsWorld::addObject(Object *object)
{
  this->objects.push_back(object);
}
void PhysicsWorld::addAtmosphere(Atmosphere *atmosphere)
{
  this->atmospheres.push_back(atmosphere);
}
void PhysicsWorld::addPlanetarObject(std::unique_ptr<Planet> planetarObject)
{
  this->planetarObjectViews.push_back(planetarObject.get());
  this->planetarObjects.push_back(std::move(planetarObject));
}
void PhysicsWorld::addIntegratableObject(Integratable *object)
{
  this->integratableObjects.push_back(object);
}
void PhysicsWorld::addSun(Star *sun)
{
  if (this->sun)
    Logger::logFatal("PhysicsWorld", "Sun already exists");

  this->sun = sun;
}
void PhysicsWorld::addStar(std::unique_ptr<Star> star)
{
  this->stars.push_back(std::move(star));
}
void PhysicsWorld::addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem)
{
  this->asteroidSystemViews.push_back(asteroidSystem.get());
  this->asteroidSystems.push_back(std::move(asteroidSystem));
}
void PhysicsWorld::addSystem(System *system)
{
  this->systems.push_back(system);
}
const Star &PhysicsWorld::getSun() const
{
  if (!this->sun)
    Logger::logFatal("PhysicsWorld", "No sun");

  return *this->sun;
};
const std::vector<AsteroidSystem *> &PhysicsWorld::getAsteroidSystems() const
{
  if (this->asteroidSystemViews.empty())
    Logger::logWarning("PhysicsWorld", "Asteroid systems are empty");

  return this->asteroidSystemViews;
};
const std::vector<Planet *> &PhysicsWorld::getPlanetarObjects() const
{
  if (this->planetarObjectViews.empty())
    Logger::logWarning("PhysicsWorld", "Planets are empty");

  return this->planetarObjectViews;
};

const std::vector<Object *> &PhysicsWorld::getObjects() const
{
  return this->objects;
};

const std::vector<System *> &PhysicsWorld::getSystems() const
{
  return this->systems;
};