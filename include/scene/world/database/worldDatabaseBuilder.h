#pragma once

#include "scene/world/database/worldDatabase.h"

class Object;
class OrbitalObject;
class Model;
class InstanceManager;
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

  void processSystem(System *system, TemporaryStorage<Real> &objectStorage, TemporaryStorage<Real> &orbitalStorage, std::atomic_size_t &objectIndex, std::atomic_size_t &orbitalIndex);

  size_t findCentralBodyIndex(Object *central);

  void createObjectModel(Model &model, Object &object, float importance);

public:
  WorldDatabaseBuilder(EntityManager &entityManager, TrailManager &trailManager, Importance &importance) : entityManager(entityManager), trailManager(trailManager), importance(importance) {};
  ~WorldDatabaseBuilder() = default;

  OrbitalObject *createOrbitalObject(const std::string &name, Real mu, Radii radii, Object *centralBody, const KeplerElements<Real> &keplerElements, const RotationalElements rotationalElements, Real timeAfterJD2000, GravityField gravityField = GravityField(), TidalParameters tidalParameters = TidalParameters());
  void createPlanetModel(Model &model, OrbitalObject &planet);
  void createMoonModel(Model &model, OrbitalObject &planet);
  Object *createObject(const std::string &name, Real mu, Radii radii, Real luminosity, const RotationalElements rotationalElements, Real timeAfterJD2000, Vec3<Real> pos);
  void createStarModel(Model &model, Object &object);
  void addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, OrbitalObject *planet);
  AsteroidSystem *createAsteroidSystem(const std::string &name, ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, Real innerEdge, Real outerEdge, Real timeAfterJD2000, bool enableRender);
  const Entity convertObjectToEntity(Object *object);

  WorldDatabase<Real> build(InstanceManager &instanceManager);
};

#include "scene/world/database/worldDatabaseBuilder.hpp"