#pragma once

#include "physics/asteroid.h"

#include <vector>

class Model;
class Material;
class Shader;
class Asteroid;

class AsteroidSystem
{
private:
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
  AsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge, Material *material);
  ~AsteroidSystem() = default;

  void applyObjectGravitation(Object *object);
  void render(Shader *shader);
  void update(double dt);
};