#include "physics/asteroidSystem.h"
#include "physics/constants.h"
#include "graphics/primitives/asteroidShape.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/materials/asteroidMaterial.h"
#include "graphics/model.h"
#include "maths/random.h"

#include <glm/gtc/matrix_transform.hpp>
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
    std::lock_guard<std::mutex> lock(this->mtx);
    this->instances[type].emplace_back(InstanceData{asteroid->getRenderPosition()});
    this->asteroids[type].emplace_back(std::move(asteroid));
  }

  return ptr;
}
void AsteroidSystem::createAsteroids(unsigned amount)
{
  std::vector<std::unique_ptr<AsteroidShape>> asteroids;
  asteroids.push_back(std::make_unique<AsteroidShape>(24, 12, 2.0, 1.0, 1.0, 0.75, 0.85, 0.65));
  asteroids.push_back(std::make_unique<AsteroidShape>(20, 10, 2.2, 1.0, 1.1, 0.70, 0.80, 0.60));
  asteroids.push_back(std::make_unique<AsteroidShape>(16, 8, 3.0, 1.0, 1.2, 0.65, 0.75, 0.55));
  asteroids.push_back(std::make_unique<AsteroidShape>(4, 7, 2.5, 1.1, 1.3, 0.60, 0.70, 0.50));
  asteroids.push_back(std::make_unique<AsteroidShape>(12, 6, 4.0, 1.2, 1.4, 0.55, 0.65, 0.45));

  const size_t typeCount = asteroids.size();

  this->instances.resize(typeCount);
  this->asteroids.resize(typeCount);
  this->meshes.reserve(typeCount);

  for (auto &asteroid : asteroids)
  {
    this->meshes.push_back(std::make_unique<Mesh>(std::move(asteroid), VertexLayout::Instanced));
  }

  for (auto &mesh : meshes)
  {
    meshVolumes.push_back(mesh->calculateVolume());
  }

  unsigned int threadCount = std::thread::hardware_concurrency();
  if (threadCount == 0)
    threadCount = 1;

  std::vector<std::thread> threads;
  threads.reserve(threadCount);
  unsigned perThread = amount / threadCount;
  unsigned remaining = amount % threadCount;
  for (unsigned int i = 0; i < threadCount; i++)
  {
    threads.emplace_back([this, perThread]()
                         { 
        std::cout << "Creating asteroids in thread " << std::this_thread::get_id() << std::endl; 
        for (unsigned j = 0; j < perThread; j++) 
          this->createAsteroid(); });
  }

  for (unsigned i = 0; i < remaining; i++)
  {
    this->createAsteroid();
  }

  for (auto &t : threads)
  {
    t.join();
  }

  std::cout << "Finished creating asteroids." << std::endl;
  
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

      std::cout << "Asteroids of type " << type << ": " << this->instances[type].size() << std::endl;
    }
  }
}

// Constructor
AsteroidSystem::AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, Material *material)
{
  this->asteroid_material = material;
  this->centralBody = centralBody;

  this->innerEdge = innerEdge;
  this->outerEdge = outerEdge;

  this->createAsteroids(amount);
}

// Public functions
void AsteroidSystem::update(double dt)
{
  for (size_t typeIndex = 0; typeIndex < this->asteroids.size(); typeIndex++)
  {
    for (size_t asteroidIndex = 0; asteroidIndex < this->asteroids[typeIndex].size(); asteroidIndex++)
    {
      this->asteroids[typeIndex][asteroidIndex]->update(dt);
      auto pos = this->asteroids[typeIndex][asteroidIndex]->getRenderPosition();
      // std::cout << "New asteroid: " << pos.x << ' ' << pos.y << ' ' << pos.z << std::endl;
      // this->instances[typeIndex][asteroidIndex].ModelMatrix = this->asteroids[typeIndex][asteroidIndex]->getModelMatrix();
      this->instances[typeIndex][asteroidIndex].position = pos;
    }
    this->meshes[typeIndex]->updateInstanceBuffer(this->instances[typeIndex]);
  }
}

void AsteroidSystem::applyObjectGravitation(Object *object)
{
  for (auto &asteroidType : this->asteroids)
  {
    for (auto &asteroid : asteroidType)
      asteroid->applyGravitation(*object);
  }
}

void AsteroidSystem::render(Shader *shader)
{
  this->asteroid_material->sendToShader(*shader);

  for (auto &mesh : this->meshes)
  {
    mesh->renderInstanced();
  }
}