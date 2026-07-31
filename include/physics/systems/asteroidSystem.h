#pragma once

#include "physics/systems/system.h"

#include "physics/integrators/integratable.h"

#include "physics/asteroid.h"

#include "graphics/texture.h"

#include "camera/camera.h"

#include "resources/range.h"

#include "render/updatable.h"
#include "render/renderSystem.h"

#include <vector>

class Shader;
class ThreadPool;
class InstanceManager;
class ResourceManager;
class LODManager;

class AsteroidSystem : public System, public RenderSystem, public Integratable
{
private:
  ThreadPool &threadPool;
  std::vector<Range> typeRanges;

  size_t vboCount;

  float lastUpdateTime = 0.0f;

  std::vector<Asteroid> asteroids;
  std::vector<size_t> asteroidTypes;

  double innerEdge;
  double outerEdge;

  Object *centralBody;

  KeplerElements createRandomKeplerElements(double timeAfterJD2000);
  void createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, Radii typeRadii, double volume, double timeAfterJD2000);
  void createAsteroids(ResourceManager &resourceManager, unsigned int amount, double timeAfterJD2000);

  void initRanges(std::vector<unsigned int> &typeCounts);

  void forEachObjectImpl(std::function<void(Object &)> func) override;
  void forEachObjectImpl(std::function<void(Object &, size_t)> func) override;

public:
  AsteroidSystem(ResourceManager &resourceManager, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, float importance, ThreadPool &threadPool);
  ~AsteroidSystem() = default;

  void applyObjectGravitation(Object &object);

  void buildRenderQueue(RenderQueue &queue, LODManager &lod, InstanceManager &instances, const Camera &camera, Frustum *frustum, float viewportHeight) override;
  void reserveInstances(InstanceManager &instanceManager) override;

  Model* getModelFromObjectIndex(size_t i) override;
};
