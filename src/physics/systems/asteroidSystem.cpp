#include "physics/systems/asteroidSystem.h"

#include "debug/logger.h"

#include "physics/constants.h"

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
#include "render/lodManager.h"
#include "render/instanceManager.h"
#include "render/lodResult.h"
#include "render/renderQueue.h"
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

  for (AsteroidType *asteroid : asteroidShapes)
    this->models.push_back(std::move(asteroid->model));

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
  this->importance = importance;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(resourceManager, amount, timeAfterJD2000);
}

// Public functions
void AsteroidSystem::buildRenderQueue(RenderQueue &queue, LODManager &lod, InstanceManager &instances, const Camera &camera, Frustum *frustum, float viewportHeight)
{
  std::vector<std::vector<std::vector<InstanceModelMatrixParts>>> threadLocalFullInstances(this->threadPool.getThreadCount());
  for (auto &perThread : threadLocalFullInstances)
    perThread.resize(this->models.size());

  std::vector<std::vector<InstancePositionRadiusTexture>> threadLocalImpostorInstances(this->threadPool.getThreadCount());
  std::vector<std::vector<InstancePositionRadiusColor>> threadLocalPointInstances(this->threadPool.getThreadCount());

  float fov = camera.getFOV();
  std::vector<glm::vec3> colors;
  colors.resize(this->models.size());

  for (size_t i = 0; i < this->models.size(); i++)
    colors[i] = this->models[i]->getMaterial()->getTexture()->getAverageColor();

  this->threadPool.parallelFor(0, this->asteroids.size(), [this, &threadLocalFullInstances, &threadLocalImpostorInstances, &threadLocalPointInstances, &camera, &lod, &frustum, &colors, fov, viewportHeight](Range work, size_t thread)
                               {
                              auto& localFull = threadLocalFullInstances[thread];
                              auto& localImpostor = threadLocalImpostorInstances[thread];
                              auto& localPoint = threadLocalPointInstances[thread];
                               for (unsigned i = work.begin; i < work.end; i++)
                               {
                                const Asteroid& asteroid = this->asteroids[i];
                                Radii radii = asteroid.getRadii();

                                glm::dvec3 pos = camera.worldToViewSpace(asteroid.getPosition());
                                LODResult res = lod.partitionObject(pos, importance, radii, frustum, viewportHeight, fov);

                                if (!res.visible)
                                  continue;

                                switch (res.level)
                                {
                                case LOD::Full:
                                  localFull[this->asteroidTypes[i]].emplace_back(InstanceModelMatrixParts{pos, camera.worldToViewSpace(asteroid.getOrientation()), glm::vec3(res.scaledEquatorianRadius, res.scaledPolarRadius, res.scaledEquatorianRadius)});
                                  break;
                                case LOD::Impostor: 
                                  localImpostor.emplace_back(InstancePositionRadiusTexture{pos, res.scaledMeanRadius, this->models[this->asteroidTypes[i]]->getImpostorLayer()});
                                  break;
                                case LOD::Point:
                                  localPoint.emplace_back(InstancePositionRadiusColor{pos, res.scaledMeanRadius, colors[this->asteroidTypes[i]]});
                                  break;
                                default:
                                  Logger::logError("Asteroid System", "No handler for LOD level: " + std::to_string(res.level));
                                  break;
                                }
                              } });

  std::vector<std::vector<InstanceModelMatrixParts>> tempFullInstances(this->threadPool.getThreadCount());
  std::vector<InstancePositionRadiusTexture> tempImpostorInstances(this->threadPool.getThreadCount());
  std::vector<InstancePositionRadiusColor> tempPointInstances(this->threadPool.getThreadCount());

  for (auto &local : threadLocalFullInstances)
    for (size_t type = 0; type < local.size(); type++)
      tempFullInstances[type].insert(tempFullInstances[type].end(), std::make_move_iterator(local[type].begin()), std::make_move_iterator(local[type].end()));

  for (auto &local : threadLocalImpostorInstances)
    instances.add(std::move(local));

  for (auto &local : threadLocalPointInstances)
    instances.add(std::move(local));

  for (size_t type = 0; type < this->models.size(); type++)
  {
    Range range = instances.add(std::move(tempFullInstances[type]));

    if (this->models[type]->getIsTangent())
      queue.addTangentBatch({this->models[type].get(), range});
    else
      queue.addCoreBatch({this->models[type].get(), range});
  }
}

void AsteroidSystem::applyObjectGravitation(Object &object)
{
  this->threadPool.parallelFor(0, this->asteroids.size(), [this, &object](size_t i)
                               { this->asteroids[i].applyGravitation(object); });
}