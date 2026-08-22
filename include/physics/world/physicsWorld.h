#pragma once

#include "physics/world/IphysicsWorld.h"

#include "physics/world/data/physicsCPUData.h"
#include "physics/world/data/physicsGPUBuffers.h"
#include "physics/world/data/physicsDatabase.h"

#include "physics/world/backend/physicsBackend.h"

#include "resources/entity/entity.h"

#include <vector>

class Object;
class OrbitalObject;
class Planet;
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
struct IntegratorGPUBuffers;
struct Total;

template <typename Real>
class PhysicsWorld : public IPhysicsWorld
{
private:
  PhysicsGPUBuffers gpuBuffers;
  PhysicsDatabase<Real> database;
  bool wasDatabaseInit = false;

  std::unique_ptr<PhysicsBackend> backend;

  const Entity *sun;

public:
  PhysicsWorld();
  ~PhysicsWorld();

  void initGPUBuffers(Context &ctx, PhysicsDatabase<Real> data);

  void initCPUBackend();
  void initGPUBackend(ResourceManager &resourceManager, Context &ctx, CommandQueue &queue, IntegratorGPUBuffers &gpu, Total &total);

  void step(double dt);

  void addSun(const Entity &sun);
  void setDatabase(PhysicsDatabase<Real> database);

  const PhysicsGPUBuffers &getGPUBuffers() { return this->gpuBuffers; };
  const PhysicsDatabase<Real> &getDatabase() { return this->database; };

  const Entity &getSun() const override;
};

#include "physics/world/physicsWorld.hpp"