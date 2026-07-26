#pragma once

#include "scene/world/worldDataGPU.h"

class Object;
class OrbitalObject;
class Model;
class ModelSource;
struct PhysicsCPUData;
struct WorldObject;
struct WorldSystem;

template <typename Real>
class WorldGPUBuilder
{
private:
  void processObject(Object *obj, WorldDataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);

  void processOrbital(OrbitalObject *obj, WorldDataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex);

  void processModel(const Model *model, WorldDataGPU<Real> &data, size_t i);

  void processModelSource(const ModelSource *modelSource, WorldDataGPU<Real> &data, size_t i);

public:
  WorldGPUBuilder() = default;
  ~WorldGPUBuilder() = default;

  WorldDataGPU<Real> build(std::vector<WorldObject> &worldObjects, std::vector<WorldSystem> &worldSystems);
};

#include "scene/world/worldGPUBuilder.hpp"