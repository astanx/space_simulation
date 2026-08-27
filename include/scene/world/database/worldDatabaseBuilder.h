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
struct Importance;

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
  Importance &importance;
  EntityManager &entityManager;

  std::unordered_map<Object *, const Entity *> objectToEntity;

  std::mutex loveMutex;
  std::mutex tidalMutex;
  std::mutex modelMutex;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;
  Total total;
  Total modelTotal;

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
  WorldDatabaseBuilder(EntityManager &entityManager, Importance &importance) : entityManager(entityManager), importance(importance) {};
  ~WorldDatabaseBuilder() = default;

  Planet *createPlanet(Model &model, Real mu, Radii radii, Object *centralBody, const KeplerElements<Real> &keplerElements, const RotationalElements rotationalElements, Real timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters(), Real g = 0.0);
  Object *createStar(Model &model, Real mu, Radii radii, Real luminosity, const RotationalElements rotationalElements, Real timeAfterJD2000, Vec3<Real> pos);
  Moon *createMoon(Model &model, Real mu, Radii radii, Planet *centralBody, const KeplerElements<Real> &keplerElements, const RotationalElements rotationalElements, Real timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());
  void addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet);
  AsteroidSystem *createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, Real innerEdge, Real outerEdge, Real timeAfterJD2000);

  const Entity &convertObjectToEntity(Object *object);

  WorldDatabase<Real> build(InstanceManager &instanceManager);
};

#include "scene/world/database/worldDatabaseBuilder.hpp"