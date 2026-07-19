#pragma once

#include "physics/world/physicsCPUData.h"
#include "physics/world/physicsGPUData.h"
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

  template <typename Real>
  void processObject(Object *obj, DataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);
  template <typename Real>
  void processOrbital(OrbitalObject *obj, DataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);
  template<typename Real>
  void initGPUBuffers(Context &ctx);

public:
  PhysicsWorld();
  ~PhysicsWorld();

  void initGPU(ResourceManager &resourceManager, Context &ctx);

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