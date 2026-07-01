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

#include "render/frustum.h"
#include "render/lodManager.h"
#include "scene/frameContext.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>
#include <iterator>

// Private functions
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

void AsteroidSystem::createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, std::vector<InstancePositionRadius> &typeInstances, Radii typeRadii, double timeAfterJD2000)
{
  double radius = generateRandom(MINIMUM_ASTEROID_RADIUS, MAXIMUM_ASTEROID_RADIUS);

  if (radius * VISUAL_RADIUS_SCALE < 0.01)
    radius = 0.01 / VISUAL_RADIUS_SCALE;

  // double angle = generateRandom(0.0, 2.0 * M_PI);
  // double distance = generateRandom(this->innerEdge, this->outerEdge);
  // double height = generateRandom(-0.5 * AU_TO_METER, 0.5 * AU_TO_METER);

  glm::dvec3 pos(0.0);
  // pos.x = cos(angle) * distance;
  // pos.z = sin(angle) * distance;
  // pos.y = height;

  double density = generateRandom(MINIMUM_ASTEROID_DENSITY, MAXIMUM_ASTEROID_DENSITY);
  // double mu = density * this->meshVolumes[type] / (pow(VISUAL_RADIUS_SCALE, 3)) * G;
  double mu = density * this->meshVolumes[type] * G;

  {
    std::lock_guard<std::mutex> lock(this->threadPool.getMutex());
    typeAsteroids.emplace_back(this->centralBody, mu, typeRadii.scaled(radius), this->createRandomKeplerElements(timeAfterJD2000));
    typeInstances.emplace_back(InstancePositionRadius{typeAsteroids.back().getPosition(), static_cast<float>(typeRadii.scaled(600000).mean)});
  }
}
void AsteroidSystem::createAsteroids(unsigned amount, double timeAfterJD2000)
{
  std::vector<std::unique_ptr<AsteroidShape>> asteroidShapes;
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(24, 12, 2.0, 1.0, 1.0, 0.75, 0.85, 0.65, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(20, 10, 2.2, 1.0, 1.1, 0.70, 0.80, 0.60, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(16, 8, 3.0, 1.0, 1.2, 0.65, 0.75, 0.55, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(4, 7, 2.5, 1.1, 1.3, 0.60, 0.70, 0.50, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(12, 6, 4.0, 1.2, 1.4, 0.55, 0.65, 0.45, VISUAL_ASTEROID_SCALE));

  std::vector<Radii> asteroidRadii;
  for (std::unique_ptr<AsteroidShape> &asteroid : asteroidShapes)
    asteroidRadii.push_back(asteroid->getRadii());

  const size_t typeCount = asteroidShapes.size();

  this->meshes.reserve(typeCount);

  for (std::unique_ptr<AsteroidShape> &asteroid : asteroidShapes)
    this->meshes.push_back(std::make_unique<Mesh>(std::move(asteroid), VertexLayout::NoColor));

  for (std::unique_ptr<Mesh> &mesh : meshes)
    meshVolumes.push_back(mesh->calculateVolume());

  std::vector<unsigned int> typeCounts(typeCount, 0);
  this->asteroidTypes.resize(amount);

  for (size_t i = 0; i < amount; i++)
  {
    unsigned type = generateRandom(0u, static_cast<unsigned>(this->meshes.size() - 1));

    asteroidTypes[i] = type;
    typeCounts[type]++;
  }

  this->fullInstances.resize(typeCount);
  for (size_t i = 0; i < typeCount; i++)
    this->fullInstances[i].reserve(typeCounts[i]);

  this->initRanges(typeCounts);

  std::vector<std::vector<Asteroid>> tempAsteroids(typeCount);
  for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  {
    Range &work = this->threadRanges[threadIndex];
    this->threadPool.enqueue([this, threadIndex, &work, &tempAsteroids, &asteroidRadii, timeAfterJD2000]()
                             {
                                 for (unsigned int i = work.begin; i < work.end; i++)
                                   {
                                    this->createAsteroid(this->asteroidTypes[i], tempAsteroids[this->asteroidTypes[i]], this->fullInstances[asteroidTypes[i]], asteroidRadii[asteroidTypes[i]], timeAfterJD2000);
                                  } });
  }

  this->threadPool.wait();

  for (size_t type = 0; type < typeCount; type++)
    this->asteroids.insert(this->asteroids.end(), std::make_move_iterator(tempAsteroids[type].begin()), std::make_move_iterator(tempAsteroids[type].end()));

  for (unsigned type = 0; type < typeCount; type++)
  {
    Mesh *mesh = this->meshes[type].get();

    mesh->setInstanceLayout(InstanceLayout::PositionRadius);
    mesh->setInstanceBuffer(this->fullInstances[type].data(), this->fullInstances[type].size(), this->vboCount);

    Logger::logInfo("Asteroid system", "Asteroids of type \"" + std::to_string(type) + "\" created - " + std::to_string(this->fullInstances[type].size()));
  }
}

void AsteroidSystem::initRanges(std::vector<unsigned int> &typeCounts)
{
  unsigned threadCount = this->threadPool.getThreadCount();

  this->threadRanges.resize(threadCount);

  size_t start = 0;
  for (size_t typeIndex = 0; typeIndex < typeCounts.size(); typeIndex++)
  {
    this->typeRanges.push_back({start, start + typeCounts[typeIndex]});
    start += typeCounts[typeIndex];
  }

  size_t total = start;
  this->threadPool.initRanges(this->threadRanges, total);
}

// Constructor
AsteroidSystem::AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, float importance, Material *material, ThreadPool &threadPool) : threadPool(threadPool), Integratable(true)
{
  AsteroidMaterial *mat = dynamic_cast<AsteroidMaterial *>(material);
  if (!mat)
    Logger::logFatal("Asteroid System", "Wrong material");

  this->asteroid_material = mat;
  this->centralBody = centralBody;

  this->vboCount = 2;
  this->importance = importance;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(amount, timeAfterJD2000);
}

// Public functions
void AsteroidSystem::partitionObjects(std::vector<InstancePositionRadiusTexture> &impostorInstances, std::vector<InstancePositionRadiusColor> &pointInstances, const Camera &camera, LODManager *manager, float viewportHeight, Frustum *frustum, bool force)
{
  for (size_t i = 0; i < this->fullInstances.size(); i++)
    this->fullInstances[i].clear();

  std::vector<std::vector<std::vector<InstancePositionRadius>>> threadLocalFullInstances(threadRanges.size());
  for (auto &perThread : threadLocalFullInstances)
    perThread.resize(this->fullInstances.size());

  std::vector<std::vector<InstancePositionRadiusTexture>> threadLocalImpostorInstances(threadRanges.size());
  std::vector<std::vector<InstancePositionRadiusColor>> threadLocalPointInstances(threadRanges.size());

  float fov = camera.getFOV();
  glm::vec3 color = this->asteroid_material->getTexture()->getAverageColor();

  for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  {
    Range &work = this->threadRanges[threadIndex];

    this->threadPool.enqueue([this, work, &camera, &threadLocalFullInstances, &threadLocalImpostorInstances, &threadLocalPointInstances, threadIndex, manager, force, &frustum, viewportHeight, fov, color]()
                             {
                              auto& localFull = threadLocalFullInstances[threadIndex];
                              auto& localImpostor = threadLocalImpostorInstances[threadIndex];
                              auto& localPoint = threadLocalPointInstances[threadIndex];
                               for (unsigned i = work.begin; i < work.end; i++)
                               {
                                const Asteroid& asteroid = this->asteroids[i];
                                float radius = asteroid.getRadius();
                                glm::dvec3 pos = camera.worldToViewSpace(asteroid.getPosition());
                                float scaledRadius = manager->scaleRadius(pos, radius,fov, viewportHeight, this->importance);

                                if (!Frustum::shouldBeProcessed(frustum, pos, scaledRadius, force))
                                  continue;

                                int level = manager->getLODLevel(pos, radius,fov, viewportHeight, this->importance);
                                switch (level)
                                {
                                case LOD::Full: {
                                  size_t type = this->asteroidTypes[i];
                                  localFull[type].emplace_back(InstancePositionRadius{pos, scaledRadius});
                                  break;
                                }
                                case LOD::Impostor: 
                                  localImpostor.emplace_back(InstancePositionRadiusTexture{pos, scaledRadius, this->impostorLayer});
                                  break;
                                case LOD::Point:
                                  localPoint.emplace_back(InstancePositionRadiusColor{pos, scaledRadius, color});
                                  break;
                                default:
                                  Logger::logError("Asteroid System", "No handler for LOD level: " + std::to_string(level));
                                  break;
                                }
                              } });
  }
  this->threadPool.wait();

  size_t count = 0;
  for (auto &local : threadLocalFullInstances)
    for (size_t type = 0; type < local.size(); type++)
    {
      this->fullInstances[type].insert(this->fullInstances[type].end(), std::make_move_iterator(local[type].begin()), std::make_move_iterator(local[type].end()));
      count += local[type].end() - local[type].begin();
    }

  for (auto &local : threadLocalImpostorInstances)
    impostorInstances.insert(impostorInstances.end(), std::make_move_iterator(local.begin()), std::make_move_iterator(local.end()));

  for (auto &local : threadLocalPointInstances)
    pointInstances.insert(pointInstances.end(), std::make_move_iterator(local.begin()), std::make_move_iterator(local.end()));
}

void AsteroidSystem::update(const Camera &camera, Frustum *frustum, bool force)
{
  // for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  // {
  //   Range &work = this->threadRanges[threadIndex];
  //   this->threadPool.enqueue([this, work, &camera]()
  //                            {
  //                              for (unsigned j = work.begin; j < work.end; j++)
  //                                this->fullInstances[j].position = camera.worldToViewSpace(this->asteroids[j].getPosition()); });
  // }
  // this->threadPool.wait();

  for (unsigned typeIndex = 0; typeIndex < this->meshes.size(); typeIndex++)
    this->meshes[typeIndex]->updateInstanceBuffer(this->fullInstances[typeIndex].data(), this->fullInstances[typeIndex].size(), this->vboCount);
}

void AsteroidSystem::applyObjectGravitation(Object *object)
{
  for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  {
    Range &work = this->threadRanges[threadIndex];

    this->threadPool.enqueue([this, work, &object]()
                             {
        for (unsigned j = work.begin; j < work.end; j++) 
          this->asteroids[j].applyGravitation(*object); });
  }
  this->threadPool.wait();
}

void AsteroidSystem::forEachObject(std::function<void(Object &)> &&func)
{
  for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  {
    Range &work = this->threadRanges[threadIndex];

    this->threadPool.enqueue([this, work, &func]()
                             {
        for (unsigned j = work.begin; j < work.end; j++) 
          func(this->asteroids[j]); });
  }
  this->threadPool.wait();
}

void AsteroidSystem::render(Shader &shader, Frustum *frustum, bool force) const
{
  this->asteroid_material->sendToShader(shader);

  for (const std::unique_ptr<Mesh> &mesh : this->meshes)
    mesh->renderInstanced();
}

void AsteroidSystem::renderInstanced(Shader &shader, Frustum *frustum, bool force) const
{
  this->render(shader, frustum, force);
}

const Texture *AsteroidSystem::getTexture()
{
  return this->asteroid_material->getTexture();
}
