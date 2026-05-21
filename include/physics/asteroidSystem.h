#pragma once

#include "physics/asteroid.h"
#include "physics/wisdomHolman.h"

#include "render/renderable.h"
#include "render/updatable.h"

#include <vector>

class Model;
class Material;
class Shader;
class ThreadPool;

struct ThreadWork
{
  size_t begin;
  size_t end;
};

class AsteroidSystem : public Renderable, public Updatable, public WisdomHolman
{
private:
  ThreadPool &threadPool;
  std::vector<std::vector<ThreadWork>> threadRanges;
  std::vector<double> meshVolumes;

  float lastUpdateTime = 0.0f;

  std::vector<std::deque<Asteroid>> asteroids;
  Material *asteroid_material;

  std::vector<std::vector<InstanceData>> instances;
  std::vector<std::unique_ptr<Mesh>> meshes;

  double innerEdge;
  double outerEdge;

  Object *centralBody;

  KeplerElements createRandomKeplerElements();
  void createAsteroid(size_t type);
  void createAsteroids(unsigned int amount);

  void initThreadRanges(std::vector<unsigned int>& typeCounts);

public:
  AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, Material *material, ThreadPool &threadPool);
  ~AsteroidSystem() = default;

  void drift(double dt) override;
  void halfKick(const std::vector<Object *> &bodies, double dt) override;

  void applyObjectGravitation(Object *object);
  void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  void update(double dt, FrameContext &ctx, Frustum *frustum = nullptr, bool force = false) override;
};