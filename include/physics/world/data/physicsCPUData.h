#pragma once

#include <vector>

class Object;
class Atmosphere;
class Integratable;
class Planet;
class System;
class AsteroidSystem;

struct PhysicsCPUData
{
  std::vector<Object *> objects;
  std::vector<Atmosphere *> atmospheres;
  std::vector<Integratable *> integratable;
  std::vector<Object *> integratableObjects;
  std::vector<System *> integratableSystems;

  std::vector<std::unique_ptr<Planet>> planetarObjects;
  std::vector<Planet *> planetarObjectViews;

  std::vector<System *> systems;

  std::vector<std::unique_ptr<AsteroidSystem>> asteroidSystems;
  std::vector<AsteroidSystem *> asteroidSystemViews;
};