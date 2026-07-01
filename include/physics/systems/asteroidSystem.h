#pragma once

#include "physics/systems/system.h"

#include "physics/integrators/integratable.h"

#include "physics/asteroid.h"

#include "graphics/texture.h"

#include "camera/camera.h"

#include "resources/range.h"

#include "render/renderable.h"
#include "render/updatable.h"

#include <vector>

class Model;
class Material;
class AsteroidMaterial;
class Shader;
class ThreadPool;
class LODManager;

class AsteroidSystem : public Renderable, public Updatable, public System, public Integratable
{
private:
  ThreadPool &threadPool;
  std::vector<Range> threadRanges;
  std::vector<Range> typeRanges;
  std::vector<double> meshVolumes;

  size_t vboCount;

  float lastUpdateTime = 0.0f;
  float importance;

  std::vector<Asteroid> asteroids;
  std::vector<size_t> asteroidTypes;
  AsteroidMaterial *asteroid_material;

  std::vector<std::vector<InstancePositionRadius>> fullInstances;
  std::vector<std::unique_ptr<Mesh>> meshes;

  double innerEdge;
  double outerEdge;

  Object *centralBody;

  KeplerElements createRandomKeplerElements(double timeAfterJD2000);
  void createAsteroid(size_t type, std::vector<Asteroid> &typeAsteroids, std::vector<InstancePositionRadius> &typeInstances, Radii typeRadii, double timeAfterJD2000);
  void createAsteroids(unsigned int amount, double timeAfterJD2000);

  void initRanges(std::vector<unsigned int> &typeCounts);

public:
  AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000, float importance, Material *material, ThreadPool &threadPool);
  ~AsteroidSystem() = default;

  void forEachObject(std::function<void(Object &)> &&func) override;

  void applyObjectGravitation(Object *object);
  void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  void update(const Camera &camera, Frustum *frustum = nullptr, bool force = false) override;

  void partitionObjects(std::vector<InstancePositionRadiusTexture> &impostorInstances, std::vector<InstancePositionRadiusColor> &pointInstances, const Camera &camera, LODManager *manager, float viewportHeight, Frustum *frustum = nullptr, bool force = false) override;
  const Texture *getTexture() override;
};
