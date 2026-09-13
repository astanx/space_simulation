#pragma once

#include "physics/systems/system.h"

#include "physics/orbitalObject.h"

#include "physics/integrators/integratable.h"
#include "physics/structs/keplerElements.h"

#include "graphics/texture.h"

#include "camera/camera.h"

#include "resources/data/range.h"

#include "render/renderSystem.h"

#include <vector>

class Shader;
class ThreadPool;
class InstanceManager;
class ResourceManager;
class LODManager;
struct Radii;

class AsteroidSystem : public System, public RenderSystem, public Integratable
{
private:
  ThreadPool &threadPool;

  size_t vboCount;

  float lastUpdateTime = 0.0f;

  std::vector<OrbitalObject> asteroids;
  std::vector<size_t> asteroidTypes;

  double innerEdge;
  double outerEdge;

  Object *centralBody;

  KeplerElements<double> createRandomKeplerElements(double timeAfterJD2000);
  void createAsteroid(size_t type, std::vector<OrbitalObject> &typeAsteroids, Radii typeRadii, double volume, double timeAfterJD2000);
  void createAsteroid(size_t type, std::vector<OrbitalObject> &typeAsteroids, double timeAfterJD2000);
  void createAsteroids(ResourceManager &resourceManager, unsigned int amount, double timeAfterJD2000, bool enableRender);

  void forEachObjectImpl(std::function<void(Object &)> func) override;
  void forEachObjectImpl(std::function<void(Object &, size_t)> func) override;

public:
  AsteroidSystem(ResourceManager &resourceManager, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, float importance, ThreadPool &threadPool, bool enableRender);
  ~AsteroidSystem() = default;

  Model *getModelFromObjectIndex(size_t i) override;
};
