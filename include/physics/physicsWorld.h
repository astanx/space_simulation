#pragma once

#include <vector>

class Object;
class Planet;
class Star;
class AsteroidSystem;
class Integrator;
class Integratable;
class Trail;

class PhysicsWorld
{
private:
  std::unique_ptr<Integrator> integrator;

  std::vector<Object *> objects;
  std::vector<Integratable *> integratableObjects;

  std::vector<std::unique_ptr<Planet>> planetarObjects;
  std::vector<Planet *> planetarObjectViews;

  std::vector<std::unique_ptr<Star>> stars;
  Star *sun;

  std::vector<std::unique_ptr<AsteroidSystem>> asteroidSystems;
  std::vector<AsteroidSystem *> asteroidSystemViews;

public:
  PhysicsWorld();
  ~PhysicsWorld();

  void step(double dt);

  void addObject(Object *object);
  void addPlanetarObject(std::unique_ptr<Planet> planetarObject);
  void addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem);
  void addStar(std::unique_ptr<Star> star);
  void addSun(Star *sun);
  void addIntegratableObject(Integratable *object);

  const Star &getSun() const;

  const std::vector<AsteroidSystem *> &getAsteroidSystems() const;
  const std::vector<Planet *> &getPlanetarObjects() const;
};