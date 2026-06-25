#include "physics/physicsWorld.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/planet.h"
#include "physics/star.h"
#include "physics/systems/asteroidSystem.h"

#include "physics/integrators/wisdomHolman.h"

#include "debug/logger.h"

// Constructor / Destructor
PhysicsWorld::PhysicsWorld()
{
  this->integrator = std::make_unique<WisdomHolmanIntegrator>();
}
PhysicsWorld::~PhysicsWorld() = default;

// Public functions
void PhysicsWorld::step(double dt)
{
  this->integrator->step(this->integratableObjects, dt);
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
    throw std::runtime_error("[PhysicsWorld] RUNTIME ERROR: Sun already exists");

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
const Star &PhysicsWorld::getSun() const
{
  if (!this->sun)
    throw std::runtime_error("[PhysicsWorld] RUNTIME ERROR: No sun");

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