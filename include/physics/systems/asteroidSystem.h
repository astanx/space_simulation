#pragma once

#include "physics/systems/system.h"

#include "physics/integrators/integratable.h"

#include "physics/asteroid.h"

#include "camera/camera.h"

#include "render/renderable.h"
#include "render/updatable.h"

#include <vector>

class Model;
class Material;
class Shader;
class ThreadPool;

struct Range
{
  size_t begin;
  size_t end;
};

class AsteroidSystem : public Renderable, public Updatable, public System, public Integratable
{
private:
  ThreadPool &threadPool;
  std::vector<Range> threadRanges;
  std::vector<Range> typeRanges;
  std::vector<double> meshVolumes;

  size_t vboCount;

  float lastUpdateTime = 0.0f;

  std::vector<Asteroid> asteroids;
  std::vector<size_t> asteroidTypes;
  Material *asteroid_material;

  std::vector<InstanceData> instances;
  std::vector<std::unique_ptr<Mesh>> meshes;

  double innerEdge;
  double outerEdge;

  Object *centralBody;

  KeplerElements createRandomKeplerElements(double timeAfterJD2000);
  void createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, std::vector<InstanceData> &typeInstances, double timeAfterJD2000);
  void createAsteroids(unsigned int amount, double timeAfterJD2000);

  void initRanges(std::vector<unsigned int> &typeCounts);

public:
  AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, Material *material, ThreadPool &threadPool);
  ~AsteroidSystem() = default;

  void forEachObject(std::function<void(Object &)> &&func) override;

  void applyObjectGravitation(Object *object);
  void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  void update(const Camera &camera) override;
};