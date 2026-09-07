#include "physics/systems/asteroidSystem.h"

#include "debug/logger.h"

#include "physics/constants.h"

#include "maths/constants.h"

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "graphics/primitives/asteroidShape.h"
#include "graphics/primitives/quad.h"
#include "graphics/primitives/point.h"

#include "graphics/materials/asteroidMaterial.h"

#include "graphics/bindings/impostor.h"

#include "graphics/state/scopedTexture.h"

#include "maths/random.h"

#include "resources/threadPool.h"
#include "resources/resourceManager.h"
#include "resources/resources.h"
#include "resources/asteroidType.h"

#include "render/frustum.h"
#include "render/lod/manager/lodManager.h"
#include "render/instanceManager.h"
#include "render/lod/lodResult.h"
#include "render/queue/renderQueue.h"
#include "render/queue/builder/renderQueueBuilder.h"
#include "render/queue/renderBatch.h"

#include "scene/frameContext.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <thread>
#include <iterator>

// Private functions
void AsteroidSystem::forEachObjectImpl(std::function<void(Object &)> func)
{
  this->threadPool.parallelFor(0, this->asteroids.size(),
                               [this, &func](size_t i)
                               {
                                 func(this->asteroids[i]);
                               });
}

void AsteroidSystem::forEachObjectImpl(std::function<void(Object &, size_t)> func)
{
  this->threadPool.parallelFor(0, this->asteroids.size(),
                               [this, &func](size_t i)
                               {
                                 func(this->asteroids[i], i);
                               });
}

KeplerElements<double> AsteroidSystem::createRandomKeplerElements(double timeAfterJD2000)
{
  KeplerElements<double> e{
      generateRandom(this->innerEdge, this->outerEdge),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.e, MAXIMUM_ASTEROID_ELEMENTS.e),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.i, MAXIMUM_ASTEROID_ELEMENTS.i),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.Omega, MAXIMUM_ASTEROID_ELEMENTS.Omega),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.omega, MAXIMUM_ASTEROID_ELEMENTS.omega),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.m, MAXIMUM_ASTEROID_ELEMENTS.m)};

  e.calculateMeanMotion(this->centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  return e;
}

void AsteroidSystem::createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, Radii typeRadii, double volume, double timeAfterJD2000)
{
  double radius = generateRandom(MINIMUM_ASTEROID_RADIUS, MAXIMUM_ASTEROID_RADIUS);

  if (radius < 0.01)
    radius = 0.01;

  glm::dvec3 pos(0.0);

  double density = generateRandom(MINIMUM_ASTEROID_DENSITY, MAXIMUM_ASTEROID_DENSITY);
  volume *= radius * radius * radius;
  double mu = density * volume * G;

  {
    std::lock_guard<std::mutex> lock(this->threadPool.getMutex());
    typeAsteroids.emplace_back(this->centralBody, mu, typeRadii.scaled(radius), this->createRandomKeplerElements(timeAfterJD2000));
  }
}

void AsteroidSystem::createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, double timeAfterJD2000)
{
  double polar = generateRandom(MINIMUM_ASTEROID_RADIUS, MAXIMUM_ASTEROID_RADIUS);
  double equatorian = generateRandom(MINIMUM_ASTEROID_RADIUS, MAXIMUM_ASTEROID_RADIUS);
  double scale = std::max(polar, equatorian);

  // normalize
  polar /= scale;
  equatorian /= scale;

  Radii radii{
      equatorian,
      polar,
      (2 * equatorian + polar) / 3};

  double volume = 4 / 3 * (M_PI * equatorian * equatorian * polar);

  this->createAsteroid(type, typeAsteroids, radii, volume, timeAfterJD2000);
}

void AsteroidSystem::createAsteroids(ResourceManager &resourceManager, unsigned amount, double timeAfterJD2000, bool enableRender)
{
  this->asteroidTypes.resize(amount);
  size_t typeCount = 1;
  std::vector<std::vector<Asteroid>> tempAsteroids(typeCount);

  this->totalObjects = amount;

  if (enableRender)
  {
    std::vector<AsteroidType *> asteroidShapes;
    asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::EROS_ASTEROID));
    asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::ITOKAWA_ASTEROID));
    asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::BENNU_ASTEROID));
    asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::RYUGU_ASTEROID));
    asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::VESTA_ASTEROID));

    typeCount = asteroidShapes.size();
    tempAsteroids.resize(typeCount);
    this->models.reserve(typeCount);

    for (size_t i = 0; i < asteroidShapes.size(); i++)
      this->models.push_back(asteroidShapes[i]->model);

    std::vector<unsigned int> typeCounts(typeCount, 0);

    for (size_t i = 0; i < amount; i++)
    {
      unsigned type = generateRandom(0u, static_cast<unsigned>(this->models.size() - 1));

      asteroidTypes[i] = type;
      typeCounts[type]++;
    }

    this->threadPool.parallelFor(0, this->asteroidTypes.size(), [this, &tempAsteroids, &asteroidShapes, timeAfterJD2000](size_t i)
                                 { this->createAsteroid(this->asteroidTypes[i], tempAsteroids[this->asteroidTypes[i]], asteroidShapes[asteroidTypes[i]]->radii, asteroidShapes[asteroidTypes[i]]->volume, timeAfterJD2000); });

    for (unsigned type = 0; type < typeCount; type++)
      Logger::logInfo("Asteroid system", "Asteroids of type \"" + std::to_string(type) + "\" created - " + std::to_string(typeCounts[type]));
  }
  else
    this->threadPool.parallelFor(0, this->asteroidTypes.size(), [this, &tempAsteroids, timeAfterJD2000](size_t i)
                                 { this->createAsteroid(this->asteroidTypes[i], tempAsteroids[this->asteroidTypes[i]], timeAfterJD2000); });

  for (size_t type = 0; type < typeCount; type++)
    this->asteroids.insert(this->asteroids.end(), std::make_move_iterator(tempAsteroids[type].begin()), std::make_move_iterator(tempAsteroids[type].end()));
}

// Constructor
AsteroidSystem::AsteroidSystem(ResourceManager &resourceManager, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, float importance, ThreadPool &threadPool, bool enableRender) : threadPool(threadPool), Integratable(true)
{
  this->centralBody = centralBody;

  this->vboCount = 2;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(resourceManager, amount, timeAfterJD2000, enableRender);

  for (Model *model : this->models)
    model->setImportance(importance);
}

// Public functions
Model *AsteroidSystem::getModelFromObjectIndex(size_t i)
{
  size_t type = this->asteroidTypes[i];
  if (type >= this->models.size())
    Logger::logFatal("Asteroid System", "Asteroid type exceeds models size");

  return this->models[this->asteroidTypes[i]];
}