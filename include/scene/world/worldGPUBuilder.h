#pragma once

#include "scene/world/worldDataGPU.h"

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
class WorldGPUBuilder
{
private:
  void processObject(Object *obj, WorldDataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);
  void processOrbital(OrbitalObject *obj, WorldDataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);

  void processModel(std::vector<size_t> &modelCapacities, LookupTable &lookup, Model *model, WorldDataGPU<Real> &data, size_t i, std::mutex &modelMutex);
  void processModelSource(std::vector<size_t> &modelCapacities, LookupTable &lookup, ModelSource *modelSource, WorldDataGPU<Real> &data, size_t i, std::mutex &modelMutex);

public:
  WorldGPUBuilder() = default;
  ~WorldGPUBuilder() = default;

  WorldDataGPU<Real> build(std::vector<WorldObject> &worldObjects, std::vector<WorldSystem> &worldSystems, InstanceManager &instanceManager);
};

#include "scene/world/worldGPUBuilder.hpp"