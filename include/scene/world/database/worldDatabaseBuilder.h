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
struct TemporaryStorage
{
  WorldDatabase<Real> database;
  LookupTable lookup;
  std::vector<size_t> modelCapacities;
};

template <typename Real>
class WorldDatabaseBuilder
{
private:
  std::mutex loveMutex;
  std::mutex tidalMutex;
  std::mutex modelMutex;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;
  Total total;

  std::vector<WorldObject> worldObjects;
  std::vector<WorldOrbitalObject> worldOrbitalObjects;
  std::vector<WorldSystem> worldSystems;

  std::vector<std::unique_ptr<System>> systems;
  std::vector<std::unique_ptr<Object>> objects;

  void processObject(Object *obj, WorldDatabase<Real> &data, size_t i);
  void processOrbital(OrbitalObject *obj, WorldDatabase<Real> &data, size_t i);

  void processModel(TemporaryStorage<Real> &storage, Model *model, size_t i);
  void processModelSource(TemporaryStorage<Real> &storage, ModelSource *modelSource, size_t i);

  void processSystem(WorldSystem &system, TemporaryStorage<Real> &objectStorage, TemporaryStorage<Real> &orbitalStorage, std::atomic_size_t &objectIndex, std::atomic_size_t &orbitalIndex);

  size_t findCentralBodyIndex(Object *central);

public:
  WorldDatabaseBuilder() = default;
  ~WorldDatabaseBuilder() = default;

  Planet *createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters(), double g = 0.0);
  Star *createStar(Model &model, double mu, Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position = glm::dvec3(0.0), glm::dvec3 velocity = glm::dvec3(0.0));
  Moon *createMoon(Model &model, double mu, Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());
  void addAtmosphereToPlanet(ResourceManager &resourceManager, std::string planetName, Planet *planet);
  void addLayerToModelSource(Model &model, ModelSource *object);
  AsteroidSystem *createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000);

  WorldDatabase<Real> build(InstanceManager &instanceManager);
};

#include "scene/world/database/worldDatabaseBuilder.hpp"