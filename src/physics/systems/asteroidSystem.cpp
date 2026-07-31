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
#include "render/renderBatch.h"

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

KeplerElements AsteroidSystem::createRandomKeplerElements(double timeAfterJD2000)
{
  KeplerElements e{
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
  double mu = density * volume * G;

  {
    std::lock_guard<std::mutex> lock(this->threadPool.getMutex());
    typeAsteroids.emplace_back(this->centralBody, mu, typeRadii.scaled(radius), this->createRandomKeplerElements(timeAfterJD2000));
  }
}

void AsteroidSystem::createAsteroids(ResourceManager &resourceManager, unsigned amount, double timeAfterJD2000)
{
  std::vector<AsteroidType *> asteroidShapes;
  asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::EROS_ASTEROID));
  asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::ITOKAWA_ASTEROID));
  asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::BENNU_ASTEROID));
  asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::RYUGU_ASTEROID));
  asteroidShapes.push_back(&resourceManager.GetAsteroid(Res::VESTA_ASTEROID));

  const size_t typeCount = asteroidShapes.size();
  this->models.reserve(typeCount);

  for (size_t i = 0; i < asteroidShapes.size(); i++)
    this->models.push_back(std::move(asteroidShapes[i]->model));

  std::vector<unsigned int> typeCounts(typeCount, 0);
  this->asteroidTypes.resize(amount);

  for (size_t i = 0; i < amount; i++)
  {
    unsigned type = generateRandom(0u, static_cast<unsigned>(this->models.size() - 1));

    asteroidTypes[i] = type;
    typeCounts[type]++;
  }

  this->initRanges(typeCounts);

  std::vector<std::vector<Asteroid>> tempAsteroids(typeCount);
  this->threadPool.parallelFor(0, this->asteroidTypes.size(), [this, &tempAsteroids, &asteroidShapes, timeAfterJD2000](size_t i)
                               { this->createAsteroid(this->asteroidTypes[i], tempAsteroids[this->asteroidTypes[i]], asteroidShapes[asteroidTypes[i]]->radii, asteroidShapes[asteroidTypes[i]]->volume, timeAfterJD2000); });

  for (size_t type = 0; type < typeCount; type++)
    this->asteroids.insert(this->asteroids.end(), std::make_move_iterator(tempAsteroids[type].begin()), std::make_move_iterator(tempAsteroids[type].end()));

  for (unsigned type = 0; type < typeCount; type++)
    Logger::logInfo("Asteroid system", "Asteroids of type \"" + std::to_string(type) + "\" created - " + std::to_string(typeCounts[type]));
}

void AsteroidSystem::initRanges(std::vector<unsigned int> &typeCounts)
{
  unsigned threadCount = this->threadPool.getThreadCount();

  size_t start = 0;
  for (size_t typeIndex = 0; typeIndex < typeCounts.size(); typeIndex++)
  {
    this->typeRanges.push_back({start, start + typeCounts[typeIndex]});
    start += typeCounts[typeIndex];
  }

  this->totalObjects = start;
}

// Constructor
AsteroidSystem::AsteroidSystem(ResourceManager &resourceManager, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, float importance, ThreadPool &threadPool) : threadPool(threadPool), Integratable(true)
{
  this->centralBody = centralBody;

  this->vboCount = 2;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(resourceManager, amount, timeAfterJD2000);

  for (Model *model : this->models)
    model->setImportance(importance);
}

// Public functions
void AsteroidSystem::buildRenderQueue(RenderQueue &queue, LODManager &lod, InstanceManager &instances, const Camera &camera, Frustum *frustum, float viewportHeight)
{
  std::vector<RenderQueueBuilder> threadLocalBuilders(this->threadPool.getThreadCount(), RenderQueueBuilder(this->models));

  float fov = camera.getFOV();

  this->threadPool.parallelFor(0, this->asteroids.size(), [this, &threadLocalBuilders, &camera, &lod, &frustum, fov, viewportHeight](Range work, size_t thread)
                               {
                              auto& localBuilder = threadLocalBuilders[thread];
                              for (unsigned i = work.begin; i < work.end; i++)
                              {
                                const Asteroid& asteroid = this->asteroids[i];
                                Radii radii = asteroid.getRadii();

                                Transform transform;
                                transform.position = camera.worldToViewSpace(asteroid.getPosition());
                                transform.orientation = camera.worldToViewSpace(asteroid.getOrientation());
                                LODResult res = lod.partitionObject(transform.position, this->models[this->asteroidTypes[i]]->getImportance(), radii, frustum, viewportHeight, fov);

                                localBuilder.submit(this->models[this->asteroidTypes[i]], res, transform);
                              } });

  RenderQueueBuilder finalBuilder(this->models);
  for (auto &builder : threadLocalBuilders)
    finalBuilder.merge(builder);

  finalBuilder.finish(instances, queue);
}

Model *AsteroidSystem::getModelFromObjectIndex(size_t i)
{
  return this->models[this->asteroidTypes[i]];
}

void AsteroidSystem::applyObjectGravitation(Object &object)
{
  this->threadPool.parallelFor(0, this->asteroids.size(), [this, &object](size_t i)
                               { this->asteroids[i].applyGravitation(object); });
}
