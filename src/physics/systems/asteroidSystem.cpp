#include "physics/systems/asteroidSystem.h"

#include "debug/logger.h"

#include "physics/constants.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "graphics/primitives/asteroidShape.h"

#include "graphics/materials/asteroidMaterial.h"

#include "maths/random.h"

#include "resources/threadPool.h"

#include "render/frustum.h"

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

void AsteroidSystem::createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, std::vector<InstanceData> &typeInstances, Radii typeRadii, double timeAfterJD2000)
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
    typeInstances.emplace_back(InstanceData{typeAsteroids.back().getPosition(), static_cast<float>(typeRadii.scaled(radius * VISUAL_RADIUS_SCALE).mean)});
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

  this->initRanges(typeCounts);

  std::vector<std::vector<Asteroid>> tempAsteroids(typeCount);
  std::vector<std::vector<InstanceData>> tempInstances(typeCount);
  for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  {
    Range &work = this->threadRanges[threadIndex];
    this->threadPool.enqueue([this, threadIndex, &work, &tempAsteroids, &tempInstances, &asteroidRadii, timeAfterJD2000]()
                             {
                                 for (unsigned int i = work.begin; i < work.end; i++)
                                   {
                                    this->createAsteroid(this->asteroidTypes[i], tempAsteroids[this->asteroidTypes[i]], tempInstances[asteroidTypes[i]], asteroidRadii[asteroidTypes[i]], timeAfterJD2000);
                                  } });
  }

  this->threadPool.wait();

  for (size_t type = 0; type < typeCount; type++)
  {
    this->asteroids.insert(this->asteroids.end(), std::make_move_iterator(tempAsteroids[type].begin()), std::make_move_iterator(tempAsteroids[type].end()));
    this->instances.insert(this->instances.end(), std::make_move_iterator(tempInstances[type].begin()), std::make_move_iterator(tempInstances[type].end()));
  }

  for (unsigned type = 0; type < typeCount; type++)
  {
    Mesh *mesh = this->meshes[type].get();

    mesh->setInstanceBuffer(tempInstances[type].data(), tempInstances[type].size(), this->vboCount);

    Logger::logInfo("Asteroid system", "Asteroids of type \"" + std::to_string(type) + "\" created - " + std::to_string(tempInstances[type].size()));
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
AsteroidSystem::AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, Material *material, ThreadPool &threadPool) : threadPool(threadPool), Integratable(true)
{
  this->asteroid_material = material;
  this->centralBody = centralBody;

  this->vboCount = 2;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(amount, timeAfterJD2000);
}

// Public functions
void AsteroidSystem::update(const Camera &camera)
{
  for (size_t threadIndex = 0; threadIndex < this->threadRanges.size(); threadIndex++)
  {
    Range &work = this->threadRanges[threadIndex];
    this->threadPool.enqueue([this, work, &camera]()
                             {
                               for (unsigned j = work.begin; j < work.end; j++)
                                 this->instances[j].position = camera.worldToViewSpace(this->asteroids[j].getPosition()); });
  }
  this->threadPool.wait();

  for (unsigned typeIndex = 0; typeIndex < this->meshes.size(); typeIndex++)
  {
    Range &range = this->typeRanges[typeIndex];
    this->meshes[typeIndex]->updateInstanceBuffer(this->instances.data() + range.begin, range.end - range.begin, this->vboCount);
  }
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