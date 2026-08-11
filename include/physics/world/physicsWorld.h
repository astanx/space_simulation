#pragma once

#include "physics/world/data/physicsCPUData.h"
#include "physics/world/data/physicsGPUData.h"
#include "physics/world/data/physicsDataGPU.h"

#include "physics/world/backend/physicsBackend.h"

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
class CommandQueue;
struct IntegratorGPUData;
struct Total;

class PhysicsWorld
{
private:
  PhysicsCPUData cpu;
  PhysicsGPUData gpu;

  std::unique_ptr<PhysicsBackend> backend;

  Star *sun;

public:
  PhysicsWorld();
  ~PhysicsWorld();

  template <typename Real>
  void initGPUBuffers(Context &ctx, PhysicsDataGPU<Real> data);

  void initCPUBackend();
  void initGPUBackend(ResourceManager &resourceManager, Context &ctx, CommandQueue &queue, IntegratorGPUData &gpu, Total &total);

  void step(double dt);

  void addObject(Object *object);
  void addAtmosphere(Atmosphere *atmosphere);
  void addPlanetarObject(std::unique_ptr<Planet> planetarObject);
  void addAsteroidSystem(std::unique_ptr<AsteroidSystem> asteroidSystem);
  void addSystem(System *system);
  void addStar(std::unique_ptr<Star> star);
  void addSun(Star *sun);
  void addIntegratable(Integratable *object);

  PhysicsGPUData &getGPUData() { return this->gpu; };

  const Star &getSun() const;

  const std::vector<AsteroidSystem *> &getAsteroidSystems() const;
  const std::vector<Planet *> &getPlanetarObjects() const;
  const std::vector<Object *> &getObjects() const;
  const std::vector<System *> &getSystems() const;
};

#include "physics/world/physicsWorld.hpp"