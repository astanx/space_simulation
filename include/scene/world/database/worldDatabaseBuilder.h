#pragma once

#include "scene/world/database/worldDatabase.h"

class Object;
class OrbitalObject;
class Model;
class ModelSource;
class instanceManager;
struct PhysicsCPUData;
struct WorldObject;
struct WorldSystem;

struct LookupTable
{
  std::unordered_map<Model *, size_t> table;
  std::vector<Model *> models;
  size_t freeIndex = 0;
};

template <typename Real>
class WorldDatabaseBuilder
{
private:
  void processObject(Object *obj, WorldDatabase<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);
  void processOrbital(OrbitalObject *obj, WorldDatabase<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);

  void processModel(std::vector<size_t> &modelCapacities, LookupTable &lookup, Model *model, WorldDatabase<Real> &data, size_t i, std::mutex &modelMutex);
  void processModelSource(std::vector<size_t> &modelCapacities, LookupTable &lookup, ModelSource *modelSource, WorldDatabase<Real> &data, size_t i, std::mutex &modelMutex);

public:
  WorldDatabaseBuilder() = default;
  ~WorldDatabaseBuilder() = default;

  WorldDatabase<Real> build(std::vector<WorldObject> &worldObjects, std::vector<WorldSystem> &worldSystems, InstanceManager &instanceManager);
};

#include "scene/world/database/worldDatabaseBuilder.hpp"