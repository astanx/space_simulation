#include "physics/asteroidSystem.h"

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

// Private functions
KeplerElements AsteroidSystem::createRandomKeplerElements()
{
  return KeplerElements{
      generateRandom(this->innerEdge, this->outerEdge),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.e, MAXIMUM_ASTEROID_ELEMENTS.e),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.i, MAXIMUM_ASTEROID_ELEMENTS.i),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.Omega, MAXIMUM_ASTEROID_ELEMENTS.Omega),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.omega, MAXIMUM_ASTEROID_ELEMENTS.omega),
      generateRandom(MINIMUM_ASTEROID_ELEMENTS.m, MAXIMUM_ASTEROID_ELEMENTS.m)};
}

void AsteroidSystem::createAsteroid(size_t type)
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
  double mu = density * this->meshVolumes[type] / VISUAL_RADIUS_SCALE * G;

  std::unique_ptr<Asteroid> asteroid = std::make_unique<Asteroid>(this->centralBody, mu, radius, this->createRandomKeplerElements());

  {
    std::lock_guard<std::mutex> lock(this->threadPool.getMutex());
    this->asteroids[type].emplace_back(this->centralBody, mu, radius, this->createRandomKeplerElements());
    this->instances[type].emplace_back(InstanceData{this->asteroids[type].back().getRenderPosition()});
  }
}
void AsteroidSystem::createAsteroids(unsigned amount)
{
  std::vector<std::unique_ptr<AsteroidShape>> asteroidShapes;
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(24, 12, 2.0, 1.0, 1.0, 0.75, 0.85, 0.65, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(20, 10, 2.2, 1.0, 1.1, 0.70, 0.80, 0.60, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(16, 8, 3.0, 1.0, 1.2, 0.65, 0.75, 0.55, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(4, 7, 2.5, 1.1, 1.3, 0.60, 0.70, 0.50, VISUAL_ASTEROID_SCALE));
  asteroidShapes.push_back(std::make_unique<AsteroidShape>(12, 6, 4.0, 1.2, 1.4, 0.55, 0.65, 0.45, VISUAL_ASTEROID_SCALE));

  const size_t typeCount = asteroidShapes.size();

  this->instances.resize(typeCount);
  this->asteroids.resize(typeCount);
  this->meshes.reserve(typeCount);

  for (std::unique_ptr<AsteroidShape> &asteroid : asteroidShapes)
    this->meshes.push_back(std::make_unique<Mesh>(std::move(asteroid), VertexLayout::Instanced));

  for (std::unique_ptr<Mesh> &mesh : meshes)
    meshVolumes.push_back(mesh->calculateVolume());

  std::vector<unsigned int> typeCounts(typeCount, 0);
  std::vector<unsigned int> asteroidTypes(amount);

  for (size_t i = 0; i < amount; i++)
  {
    unsigned type = generateRandom(0u, static_cast<unsigned>(this->meshes.size() - 1));

    asteroidTypes[i] = type;
    typeCounts[type]++;
  }

  for (size_t typeIndex = 0; typeIndex < typeCount; typeIndex++)
  {
    // this->asteroids[typeIndex].reserve(typeCounts[typeIndex]);
    this->instances[typeIndex].resize(typeCounts[typeIndex]);
  }

  this->initThreadRanges();

  for (size_t typeIndex = 0; typeIndex < this->threadRanges.size(); typeIndex++)
    for (ThreadWork &work : this->threadRanges[typeIndex])
    {
      this->threadPool.enqueue([this, typeIndex, work]()
                               {
                                 for (unsigned int i = work.begin; i < work.end; i++)
                                   this->createAsteroid(typeIndex); });
    }

  this->threadPool.wait();

  for (unsigned type = 0; type < typeCount; type++)
  {
    if (!instances[type].empty())
    {
      Mesh *mesh = this->meshes[type].get();
      mesh->setInstanceBuffer(this->instances[type]);

      Logger::logInfo("Asteroid system", "Asteroids of type \"" + std::to_string(type) + "\" created - " + std::to_string(this->instances[type].size()));
    }
  }
}

void AsteroidSystem::initThreadRanges()
{
  unsigned threadCount = this->threadPool.getThreadCount();

  this->threadRanges.resize(this->asteroids.size());

  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    unsigned perThread = this->asteroids[typeIndex].size() / threadCount;
    unsigned remaining = this->asteroids[typeIndex].size() % threadCount;

    this->threadRanges[typeIndex].resize(threadCount);

    unsigned start = 0;
    for (unsigned int i = 0; i < threadCount; i++)
    {
      unsigned work = perThread + (i < remaining ? 1 : 0);
      unsigned begin = start;
      unsigned end = begin + work;

      this->threadRanges[typeIndex][i] = {begin, end};

      start = end;
    }
  }
}

// Constructor
AsteroidSystem::AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, Material *material, ThreadPool &threadPool) : threadPool(threadPool)
{
  this->asteroid_material = material;
  this->centralBody = centralBody;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(amount);
}

// Public functions
void AsteroidSystem::update(double dt, FrameContext &ctx, Frustum *frustum, bool force)
{
  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    for (ThreadWork &work : this->threadRanges[typeIndex])
    {
      this->threadPool.enqueue([this, work, dt, typeIndex, frustum, force, &ctx]()
                               {
        for (unsigned j = work.begin; j < work.end; j++)
        {
          Asteroid& asteroid = this->asteroids[typeIndex][j];
          asteroid.update(dt, ctx, frustum, force);
          this->instances[typeIndex][j].position = asteroid.getRenderPosition();
        } });
    }
  }
  this->threadPool.wait();

  for (unsigned typeIndex = 0; typeIndex < this->meshes.size(); typeIndex++)
    this->meshes[typeIndex]->updateInstanceBuffer(this->instances[typeIndex]);
}

void AsteroidSystem::applyObjectGravitation(Object *object)
{
  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    for (ThreadWork &work : this->threadRanges[typeIndex])
    {
      this->threadPool.enqueue([this, typeIndex, work, object]()
                               {
        for (unsigned j = work.begin; j < work.end; j++) 
          this->asteroids[typeIndex][j].applyGravitation(*object); });
    }
  }
  this->threadPool.wait();
}

void AsteroidSystem::drift(double dt)
{
  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    for (ThreadWork &work : this->threadRanges[typeIndex])
    {
      this->threadPool.enqueue([this, typeIndex, work, dt]()
                               {
        for (unsigned j = work.begin; j < work.end; j++) 
          this->asteroids[typeIndex][j].drift(dt); });
    }
  }
  this->threadPool.wait();
}
void AsteroidSystem::halfKick(const std::vector<Object *> &bodies, double dt)
{
  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    for (ThreadWork &work : this->threadRanges[typeIndex])
    {
      this->threadPool.enqueue([this, typeIndex, work, bodies, dt]()
                               {
        for (unsigned j = work.begin; j < work.end; j++) 
          this->asteroids[typeIndex][j].halfKick(bodies, dt); });
    }
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