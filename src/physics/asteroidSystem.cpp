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

Asteroid *AsteroidSystem::createAsteroid()
{
  unsigned type = generateRandom(0u, static_cast<unsigned>(this->meshes.size() - 1));

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

  Asteroid *ptr = asteroid.get();

  {
    std::lock_guard<std::mutex> lock(this->threadPool.getMutex());
    this->instances[type].emplace_back(InstanceData{asteroid->getRenderPosition()});
    this->asteroids[type].emplace_back(std::move(asteroid));
  }

  return ptr;
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
  {
    this->meshes.push_back(std::make_unique<Mesh>(std::move(asteroid), VertexLayout::Instanced));
  }

  for (std::unique_ptr<Mesh> &mesh : meshes)
  {
    meshVolumes.push_back(mesh->calculateVolume());
  }

  unsigned threadCount = this->threadPool.getThreadCount();
  unsigned perThread = amount / threadCount;
  unsigned remaining = amount % threadCount;
  for (unsigned int i = 0; i < threadCount; i++)
  {
    this->threadPool.enqueue([this, perThread]()
                             { 
        for (unsigned j = 0; j < perThread; j++) 
          this->createAsteroid(); });
  }

  for (unsigned i = 0; i < remaining; i++)
  {
    this->createAsteroid();
  }

  this->threadPool.wait();

  // for (size_t i = 0; i < amount; i++)
  // {

  //   Asteroid *asteroid = createAsteroid();

  //   // glm::mat4 model(1.0f);
  //   // model = glm::translate(model, glm::vec3(asteroid->getRenderPosition()));
  //   // model = glm::scale(model, glm::vec3(asteroid->getRadius() * VISUAL_RADIUS_SCALE));
  //   // this->instances[type].emplace_back(InstanceData{model});
  // }

  for (unsigned type = 0; type < asteroids.size(); type++)
  {
    if (!instances[type].empty())
    {
      Mesh *mesh = this->meshes[type].get();
      mesh->setInstanceBuffer(this->instances[type]);

      Logger::logInfo("Asteroid system", "Asteroids of type \"" + std::to_string(type) + "\" created - " + std::to_string(this->instances[type].size()));
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
  unsigned threadCount = this->threadPool.getThreadCount();

  // std::vector<std::vector<size_t>> counts(this->asteroids.size(), std::vector<size_t>(threadCount, 0));

  // // Calculate counts for each type
  // for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  // {
  //   unsigned perThread = asteroids[typeIndex].size() / threadCount;
  //   unsigned remaining = asteroids[typeIndex].size() % threadCount;

  //   unsigned start = 0;
  //   for (unsigned int i = 0; i < threadCount; i++)
  //   {
  //     unsigned work = perThread + (i < remaining ? 1 : 0);

  //     unsigned begin = start;
  //     unsigned end = begin + work;
  //     start = end;

  //     this->threadPool.enqueue([this, work, dt, typeIndex, begin, end, frustum, force, &counts, i]()
  //                              {
  //     size_t local = 0;

  //     for (unsigned j = begin; j < end; j++)
  //     {
  //       std::unique_ptr<Asteroid>& asteroid = this->asteroids[typeIndex][j];
  //       if ((frustum && frustum->isVisibleSphere(asteroid->getRenderPosition(), asteroid->getRadius() * VISUAL_RADIUS_SCALE)) || force)
  //         local++;
  //     }
  //     counts[typeIndex][i] = local; });
  //   }
  // }

  // this->threadPool.wait();

  // std::vector<size_t> totalCounts(this->asteroids.size(), 0);

  // std::vector<std::vector<size_t>> offsets(
  //     this->asteroids.size(),
  //     std::vector<size_t>(threadCount, 0));

  // for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  // {
  //   offsets[typeIndex][0] = 0;

  //   for (size_t i = 1; i < threadCount; i++)
  //   {
  //     offsets[typeIndex][i] =
  //         offsets[typeIndex][i - 1] + counts[typeIndex][i - 1];
  //   }

  //   // total = last offset + last count
  //   totalCounts[typeIndex] =
  //       offsets[typeIndex][threadCount - 1] +
  //       counts[typeIndex][threadCount - 1];
  // }

  // Update asteroids
  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    // this->instances[typeIndex].resize(totalCounts[typeIndex]);

    // std::cout << "Threads: " << threadCount << std::endl;
    unsigned perThread = asteroids[typeIndex].size() / threadCount;
    unsigned remaining = asteroids[typeIndex].size() % threadCount;

    // std::cout << "Per thread: " << perThread << ", remaining: " << remaining << std::endl;
    unsigned start = 0;
    for (unsigned int i = 0; i < threadCount; i++)
    {
      unsigned work = perThread + (i < remaining ? 1 : 0);

      unsigned begin = start;
      unsigned end = begin + work;
      start = end;

      this->threadPool.enqueue([this, work, dt, typeIndex, begin, end, frustum, force, &ctx]()
                               {
        // size_t index = offsets[typeIndex][i];
        for (unsigned j = begin; j < end; j++)
        {
          std::unique_ptr<Asteroid>& asteroid = this->asteroids[typeIndex][j];
          asteroid->update(dt, ctx, frustum, force);
          this->instances[typeIndex][j].position = asteroid->getRenderPosition() - ctx.camPosition;
        } });
    }

    // for (size_t asteroidIndex = 0; asteroidIndex < this->asteroids[typeIndex].size(); asteroidIndex++)
    // {
    //   this->instances[typeIndex][asteroidIndex].position = this->asteroids[typeIndex][asteroidIndex]->getRenderPosition();
    // }
    // this->meshes[typeIndex]->updateInstanceBuffer(this->instances[typeIndex]);
  }
  this->threadPool.wait();

  for (unsigned typeIndex = 0; typeIndex < this->meshes.size(); typeIndex++)
    this->meshes[typeIndex]->updateInstanceBuffer(this->instances[typeIndex]);
}

void AsteroidSystem::applyObjectGravitation(Object *object)
{
  float currentTime = static_cast<float>(glfwGetTime());
  if (currentTime - this->lastUpdateTime < 0.05f)
    return;
  this->lastUpdateTime = currentTime;
  unsigned threadCount = this->threadPool.getThreadCount();

  for (std::vector<std::unique_ptr<Asteroid>> &asteroidType : this->asteroids)
  {
    unsigned perThread = asteroidType.size() / threadCount;
    unsigned remaining = asteroidType.size() % threadCount;

    unsigned start = 0;
    for (unsigned int i = 0; i < threadCount; i++)
    {
      unsigned work = perThread + (i < remaining ? 1 : 0);

      unsigned begin = start;
      unsigned end = begin + work;
      start = end;

      this->threadPool.enqueue([this, &asteroidType, begin, end, object]()
                               {
        for (unsigned j = begin; j < end; j++) 
          asteroidType[j]->applyGravitation(*object); });
    }
  }
  this->threadPool.wait();
}

void AsteroidSystem::drift(double dt)
{
  float currentTime = static_cast<float>(glfwGetTime());
  if (currentTime - this->lastUpdateTime < 0.05f)
    return;
  this->lastUpdateTime = currentTime;
  unsigned threadCount = this->threadPool.getThreadCount();

  for (std::vector<std::unique_ptr<Asteroid>> &asteroidType : this->asteroids)
  {
    unsigned perThread = asteroidType.size() / threadCount;
    unsigned remaining = asteroidType.size() % threadCount;

    unsigned start = 0;
    for (unsigned int i = 0; i < threadCount; i++)
    {
      unsigned work = perThread + (i < remaining ? 1 : 0);

      unsigned begin = start;
      unsigned end = begin + work;
      start = end;

      this->threadPool.enqueue([this, &asteroidType, begin, end, dt]()
                               {
        for (unsigned j = begin; j < end; j++) 
          asteroidType[j]->drift(dt); });
    }
  }
  this->threadPool.wait();
}
void AsteroidSystem::halfKick(const std::vector<Object *> &bodies, double dt)
{
  float currentTime = static_cast<float>(glfwGetTime());
  if (currentTime - this->lastUpdateTime < 0.05f)
    return;
  this->lastUpdateTime = currentTime;
  unsigned threadCount = this->threadPool.getThreadCount();

  for (std::vector<std::unique_ptr<Asteroid>> &asteroidType : this->asteroids)
  {
    unsigned perThread = asteroidType.size() / threadCount;
    unsigned remaining = asteroidType.size() % threadCount;

    unsigned start = 0;
    for (unsigned int i = 0; i < threadCount; i++)
    {
      unsigned work = perThread + (i < remaining ? 1 : 0);

      unsigned begin = start;
      unsigned end = begin + work;
      start = end;

      this->threadPool.enqueue([this, &asteroidType, begin, end, bodies, dt]()
                               {
        for (unsigned j = begin; j < end; j++) 
          asteroidType[j]->halfKick(bodies, dt); });
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