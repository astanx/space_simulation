#pragma once

#include "physics/world/data/physicsCPUData.h"
#include "physics/world/data/physicsGPUData.h"
#include "physics/world/data/physicsDataGPU.h"
#include "physics/world/total.h"

#include <vector>

class Object;
class OrbitalObject;
class Planet;
class Star;
class AsteroidSystem;
class IntegratorCPU;
class IntegratorGPU;
class ResourceManager;
class Context;
class Integratable;
class Trail;
class Atmosphere;
class System;

class PhysicsWorld
{
private:
  PhysicsCPUData cpu;
  PhysicsGPUData gpu;

  std::unique_ptr<IntegratorCPU> integratorCPU;
  std::unique_ptr<IntegratorGPU> integratorGPU;

  Star *sun;

  Total total;

public:
  PhysicsWorld();
  ~PhysicsWorld();

  template <typename Real>
  void initGPUBuffers(Context &ctx, PhysicsDataGPU<Real> data);

  template <typename Real>
  void initGPUIntegrator(ResourceManager &resourceManager, Context &ctx, PhysicsDataGPU<Real> gpu);

  void step(double dt);

  void addObject(Object *object);
  void addAtmosphere(Atmosphere *atmosphere);
  void addPlanetarObject(std::unique_ptr<Planet> planetarObject);
  void addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem);
  void addSystem(System *system);
  void addStar(std::unique_ptr<Star> star);
  void addSun(Star *sun);
  void addIntegratable(Integratable *object);

  const Star &getSun() const;

  const std::vector<AsteroidSystem *> &getAsteroidSystems() const;
  const std::vector<Planet *> &getPlanetarObjects() const;
  const std::vector<Object *> &getObjects() const;
  const std::vector<System *> &getSystems() const;
};

#include "physics/world/physicsWorld.hpp"