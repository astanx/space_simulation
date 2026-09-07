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
  TrailManager &trailManager;

  std::unordered_map<Object *, Entity> objectToEntity;
  std::unordered_map<Object *, Model *> objectToModel;
  std::unordered_map<System *, RenderSystem *> systemToRenderSystem;

  std::mutex loveMutex;
  std::mutex tidalMutex;
  std::mutex modelMutex;
  std::mutex entityMutex;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;
  Total total;
  Total modelTotal;

  std::vector<std::unique_ptr<System>> systems;
  std::vector<std::unique_ptr<Object>> objects;
  std::vector<std::unique_ptr<OrbitalObject>> orbitalObjects;

  void processObject(Object *obj, WorldDatabase<Real> &data, size_t i);
  void processOrbital(OrbitalObject *obj, WorldDatabase<Real> &data, size_t i);

  void processModel(TemporaryStorage<Real> &storage, Model *model, size_t i);
  void processModelSource(TemporaryStorage<Real> &storage, ModelSource *modelSource, size_t i);

  void processSystem(System *system, TemporaryStorage<Real> &objectStorage, TemporaryStorage<Real> &orbitalStorage, std::atomic_size_t &objectIndex, std::atomic_size_t &orbitalIndex);

  size_t findCentralBodyIndex(Object *central);

public:
  WorldDatabaseBuilder(EntityManager &entityManager, TrailManager &trailManager, Importance &importance) : entityManager(entityManager), trailManager(trailManager), importance(importance) {};
  ~WorldDatabaseBuilder() = default;

  Planet *createPlanet(Real mu, Radii radii, Object *centralBody, const KeplerElements<Real> &keplerElements, const RotationalElements rotationalElements, Real timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters(), Real g = 0.0);
  void createPlanetModel(Model &model, Planet &planet);
  Object *createStar(Real mu, Radii radii, Real luminosity, const RotationalElements rotationalElements, Real timeAfterJD2000, Vec3<Real> pos);
  void createStarModel(Model &model, Object &object);
  Moon *createMoon(Real mu, Radii radii, Planet *centralBody, const KeplerElements<Real> &keplerElements, const RotationalElements rotationalElements, Real timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());
  void createMoonModel(Model &model, Moon &moon);
  void addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, Planet *planet);
  AsteroidSystem *createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, Real innerEdge, Real outerEdge, Real timeAfterJD2000, bool enableRender);
  const Entity convertObjectToEntity(Object *object);

  WorldDatabase<Real> build(InstanceManager &instanceManager);
};

#include "scene/world/database/worldDatabaseBuilder.hpp"