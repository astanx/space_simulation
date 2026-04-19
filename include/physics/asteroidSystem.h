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

class AsteroidSystem : public Renderable, public Updatable, public WisdomHolman
{
private:
  ThreadPool &threadPool;
  std::vector<double> meshVolumes;

  float lastUpdateTime = 0.0f;

  std::vector<std::vector<std::unique_ptr<Asteroid>>> asteroids;
  Material *asteroid_material;

  std::vector<std::vector<InstanceData>> instances;
  std::vector<std::unique_ptr<Mesh>> meshes;

  double innerEdge;
  double outerEdge;

  Object *centralBody;

  KeplerElements createRandomKeplerElements();
  Asteroid *createAsteroid();
  void createAsteroids(unsigned amount);

public:
  AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, Material *material, ThreadPool &threadPool);
  ~AsteroidSystem() = default;

  void drift(double dt) override;
  void halfKick(const std::vector<Object *> &bodies, double dt) override;

  void applyObjectGravitation(Object *object);
  void render(Shader &shader) const override;
  void renderInstanced(Shader &shader) const override;
  void update(double dt) override;
};