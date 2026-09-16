#pragma once

#include "physics/structs/radii.h"
#include "physics/systems/system.h"
#include "physics/object.h"
#include "physics/orbitalObject.h"

#include "graphics/primitives/ellipsoid.h"

#include "render/renderSystem.h"
#include "render/instanceManager.h"

#include "render/trail/trailManager.h"

// Private functions
template <typename Real>
void WorldDatabaseBuilder<Real>::processObject(Object *obj, WorldDatabase<Real> &data, size_t i)
{
  data.shared.positions[i] = static_cast<Vec3<Real>>(obj->getPosition());
  data.shared.orientations[i] = static_cast<Quat<Real>>(obj->getOrientation());
  data.shared.luminosities[i] = static_cast<Real>(obj->getLuminosity());

  Radii radii = obj->getRadii();
  data.shared.meanRadii[i] = static_cast<Real>(radii.mean);
  data.shared.polarRadii[i] = static_cast<Real>(radii.polar);
  data.shared.equatorianRadii[i] = static_cast<Real>(radii.equatorian);

  data.physics.velocities[i] = static_cast<Vec3<Real>>(obj->getVelocity());
  data.physics.mus[i] = static_cast<Real>(obj->getMu());

  data.physics.angularVelocities[i] = static_cast<Vec3<Real>>(obj->getAngularVelocity());

  data.physics.centralBodyIndices[i] = -1;

  data.physics.quadrupoleTensors[i] = static_cast<Mat3<Real>>(obj->getQuadrupoleTensor());
  data.physics.inertiaTensors[i] = static_cast<Mat3<Real>>(obj->getInertiaTensor());

  data.physics.loveIndices[i] = -1;
  data.physics.tidalFactorIndices[i] = -1;

  const TidalParameters &p = obj->getTidalParameters();
  if (p.k2 != -1)
  {
    std::lock_guard<std::mutex> lock(this->loveMutex);

    this->loveNumbers.push_back(static_cast<Real>(p.k2));
    data.physics.loveIndices[i] = loveNumbers.size() - 1;
  }
  if (p.Q != -1)
  {
    std::lock_guard<std::mutex> lock(this->tidalMutex);

    this->tidalFactors.push_back(static_cast<Real>(p.Q));
    data.physics.tidalFactorIndices[i] = tidalFactors.size() - 1;
  }
};

template <typename Real>
void WorldDatabaseBuilder<Real>::processOrbital(OrbitalObject *obj, WorldDatabase<Real> &data, size_t i)
{
  this->processObject(obj, data, i);

  const KeplerElements k = obj->getOrbit()->getKeplerElements();

  data.physics.semiAxises[i] = static_cast<Real>(k.a);
  data.physics.eccentricities[i] = static_cast<Real>(k.e);
  data.physics.inclinations[i] = static_cast<Real>(k.i);
  data.physics.longitude[i] = static_cast<Real>(k.Omega);
  data.physics.periapsis[i] = static_cast<Real>(k.omega);
  data.physics.meanAnomaly[i] = static_cast<Real>(k.m);
  data.physics.meanMotion[i] = static_cast<Real>(k.n);
};

template <typename Real>
void WorldDatabaseBuilder<Real>::processModel(TemporaryStorage<Real> &storage, Model *model, size_t i)
{
  if (!model)
    Logger::logFatal("World GPU Builder", "Model is null");

  {
    std::lock_guard<std::mutex> lock(this->modelMutex);
    size_t modelIndex;

    auto it = storage.lookup.table.find(model);
    if (it == storage.lookup.table.end())
    {
      modelIndex = storage.lookup.freeIndex++;

      storage.lookup.table[model] = modelIndex;

      storage.lookup.models.push_back(model);
      storage.database.render.isNonFullable.push_back(model->hasAnyFlag() ? 1 : 0);
      storage.database.render.modelImportances.push_back(model->getImportance());
      storage.database.render.modelColors.push_back(model->getAverageColor());
      storage.database.render.modelTextureLayers.push_back(model->getImpostorLayer());
      storage.modelCapacities.push_back(0);
    }
    else
      modelIndex = it->second;

    storage.modelCapacities[modelIndex]++;
  }
}

template <typename Real>
void WorldDatabaseBuilder<Real>::processSystem(System *system, TemporaryStorage<Real> &objectStorage, TemporaryStorage<Real> &orbitalStorage, std::atomic_size_t &objectIndex, std::atomic_size_t &orbitalIndex)
{
  system->forEachObject([this, &system, &objectStorage, &orbitalStorage, &orbitalIndex, &objectIndex](Object &obj, size_t i)
                        {
      OrbitalObject* orb = dynamic_cast<OrbitalObject*>(&obj);
      Model* model = nullptr;
      auto it = this->systemToRenderSystem.find(system);
      if (it != this->systemToRenderSystem.end())
        model = it->second->getModelFromObjectIndex(i);

      if (orb)
      {
        size_t idx = orbitalIndex.fetch_add(1);
        this->processOrbital(orb, orbitalStorage.database, idx);

        if (model)
          this->processModel(orbitalStorage, model, idx);

        {
          std::lock_guard<std::mutex> lock(this->entityMutex);
          this->entityManager.registerOrbitalEntity(this->objectToEntity.at(orb), idx);
          this->entityManager.registerObjectEntity(this->objectToEntity.at(orb), idx);

          if (model)
          {
            std::lock_guard<std::mutex> lock(this->modelMutex);
            this->entityManager.registerModelEntity(this->objectToEntity.at(orb), orbitalStorage.lookup.table[model]);
          }
          if (model && model->hasFlag(ModelFlags::Special))
          { 
            this->entityManager.registerSpecialEntity(this->objectToEntity.at(orb), idx);
            this->trailManager.registerTrail(this->objectToEntity.at(orb));
          }
        }

        Object* central = orb->getOrbit()->getCentralBody();
        orbitalStorage.database.physics.centralBodyIndices[idx] = this->findCentralBodyIndex(central);
      }
      else
      { 
        size_t idx = objectIndex.fetch_add(1);
        this->processObject(&obj, objectStorage.database, idx); 
        if (model)
          this->processModel(objectStorage, model, idx);

        {
          std::lock_guard<std::mutex> lock(this->entityMutex);
          this->entityManager.registerObjectEntity(this->objectToEntity.at(&obj), this->total.orbital + idx);

          if (model)
          {
            std::lock_guard<std::mutex> lock(this->modelMutex);
            this->entityManager.registerModelEntity(this->objectToEntity.at(&obj), this->modelTotal.orbital + objectStorage.lookup.table[model]);
          }
          if (model && model->hasFlag(ModelFlags::Special))
          {
            this->entityManager.registerSpecialEntity(this->objectToEntity.at(&obj), this->total.orbital + idx);
            this->trailManager.registerTrail(this->objectToEntity.at(&obj));
          }
        }
      } });
}

template <typename Real>
size_t WorldDatabaseBuilder<Real>::findCentralBodyIndex(Object *central)
{
  for (size_t j = 0; j < this->objects.size(); j++)
    if (this->objects[j].get() == central)
      return j + this->total.orbital;

  for (size_t j = 0; j < this->orbitalObjects.size(); j++)
    if (this->orbitalObjects[j].get() == central)
      return j;

  Logger::logFatal("World Database Builder", "Central body was not found");
  return 0;
}
template <typename Real>
void WorldDatabaseBuilder<Real>::createObjectModel(Model &model, Object &object, float importance)
{
  model.setImportance(importance);
  this->modelTotal.total++;
  this->objectToModel[&object] = &model;
}

// Public functions
template <typename Real>
const Entity WorldDatabaseBuilder<Real>::convertObjectToEntity(Object *object)
{
  if (!object)
    Logger::logFatal("World Database Builder", "Invalid object");

  auto it = this->objectToEntity.find(object);
  if (it == this->objectToEntity.end())
    Logger::logFatal("World Database Builder", "Object does not have related entity");
  return it->second;
}

template <typename Real>
void WorldDatabaseBuilder<Real>::addAtmosphereToPlanet(ResourceManager &resourceManager, ThreadPool &threadPool, std::string planetName, OrbitalObject *planet)
{
  // fix
  // std::string path = "assets/data/" + planetName + "/atmosphere/32_resolution";
  // std::unique_ptr atmosphere = std::make_unique<Atmosphere>(planet, path, threadPool);
  // Atmosphere *ptr = atmosphere.get();

  // std::unique_ptr<Ellipsoid> obj = std::make_unique<Ellipsoid>(32, atmosphere->getRadii());
  // Mesh &mesh  = resourceManager.LoadMesh<VertexPositionTexcoordNormal>(path, std::move(obj), VertexLayout::NoColor);
  // std::unique_ptr<Model> model = std::make_unique<Model>(mesh);

  // // this->physics.addAtmosphere(ptr);
  // // planet->addLayer(std::move(model));
  // planet->addAtmosphere(std::move(atmosphere));
}

template <typename Real>
OrbitalObject *WorldDatabaseBuilder<Real>::createOrbitalObject(const std::string &name, Real mu, Radii radii, Object *centralBody, const KeplerElements<Real> &keplerElements, const RotationalElements rotationalElements, Real timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters)
{
  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<OrbitalObject> obj = std::make_unique<OrbitalObject>(centralBody, mu, radii, e, tidalParameters, gravityField);

  obj->setAngularVelocity(r.calculateAngularVelocity());
  obj->setOrientation(r.calculateOrientation());

  OrbitalObject *ptr = obj.get();

  this->total.orbital++;
  this->total.total++;

  this->objectToEntity[ptr] = this->entityManager.create();
  this->entityManager.registerEntityName(this->objectToEntity[ptr], name);

  this->orbitalObjects.push_back(std::move(obj));

  return ptr;
}

template <typename Real>
Object *WorldDatabaseBuilder<Real>::createObject(const std::string &name, Real mu, Radii radii, Real luminosity, const RotationalElements rotationalElements, Real timeAfterJD2000, Vec3<Real> pos)
{
  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Object> obj = std::make_unique<Object>(mu / G, radii, TidalParameters(), GravityField(), pos);

  obj->setAngularVelocity(r.calculateAngularVelocity());
  obj->setOrientation(r.calculateOrientation());
  obj->setLuminosity(luminosity);
  obj->setMu(mu);

  Object *ptr = obj.get();

  this->total.object++;
  this->total.total++;

  this->objectToEntity[ptr] = this->entityManager.create();
  this->entityManager.registerEntityName(this->objectToEntity[ptr], name);

  this->objects.push_back(std::move(obj));

  return ptr;
}

template <typename Real>
void WorldDatabaseBuilder<Real>::createPlanetModel(Model &model, OrbitalObject &planet)
{
  this->modelTotal.orbital++;
  this->createObjectModel(model, planet, this->importance.planet);
}

template <typename Real>
void WorldDatabaseBuilder<Real>::createStarModel(Model &model, Object &star)
{
  this->modelTotal.object++;
  this->createObjectModel(model, star, this->importance.star);
}

template <typename Real>
void WorldDatabaseBuilder<Real>::createMoonModel(Model &model, OrbitalObject &moon)
{
  this->modelTotal.orbital++;
  this->createObjectModel(model, moon, this->importance.moon);
}

template <typename Real>
AsteroidSystem *WorldDatabaseBuilder<Real>::createAsteroidSystem(const std::string &name, ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, Real innerEdge, Real outerEdge, Real timeAfterJD2000, bool enableRender)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(resourceManager, centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            timeAfterJD2000, this->importance.asteroid, threadPool, enableRender);
  AsteroidSystem *ptr = system.get();

  this->total.orbital += system->getTotalObjects();
  this->total.total += system->getTotalObjects();

  std::mutex entityMutex;
  system->forEachObject([this, &entityMutex, &name](Object &obj, size_t i)
                        { 
                          std::lock_guard<std::mutex> lock(entityMutex);
                          this->objectToEntity[&obj] = this->entityManager.create();
                          this->entityManager.registerEntityName(this->objectToEntity[&obj], name + " " + std::to_string(i)); });

  if (enableRender)
  {
    this->modelTotal.orbital += system->getModels().size();
    this->modelTotal.total += system->getModels().size();

    this->systemToRenderSystem[ptr] = ptr;
  }

  this->systems.push_back(std::move(system));

  return ptr;
}

template <typename Real>
WorldDatabase<Real> WorldDatabaseBuilder<Real>::build(InstanceManager &instanceManager)
{
  TemporaryStorage<Real> objectStorage;
  TemporaryStorage<Real> orbitalStorage;

  objectStorage.database.resize(this->total.object);
  orbitalStorage.database.resize(this->total.orbital);

  size_t orbitalOffset = 0;
  size_t objectOffset = 0;

  for (std::unique_ptr<OrbitalObject> &obj : this->orbitalObjects)
  {
    this->processOrbital(obj.get(), orbitalStorage.database, orbitalOffset);
    auto it = this->objectToModel.find(obj.get());
    if (it != this->objectToModel.end())
      this->processModel(orbitalStorage, it->second, orbitalOffset);

    // orbital entity -> owns kepler/etc
    // object entity -> owns positions/basics
    this->entityManager.registerOrbitalEntity(this->objectToEntity.at(obj.get()), orbitalOffset);
    this->entityManager.registerObjectEntity(this->objectToEntity.at(obj.get()), orbitalOffset);
    if (it != this->objectToModel.end())
    {
      this->entityManager.registerModelEntity(this->objectToEntity.at(obj.get()), orbitalOffset);
      if (it->second->hasFlag(ModelFlags::Special))
      {
        this->entityManager.registerSpecialEntity(this->objectToEntity.at(obj.get()), orbitalOffset);
        this->trailManager.registerTrail(this->objectToEntity.at(obj.get()));
      }
    }

    orbitalOffset++;
  }

  for (std::unique_ptr<Object> &obj : this->objects)
  {
    this->processObject(obj.get(), objectStorage.database, objectOffset);

    auto it = this->objectToModel.find(obj.get());
    if (it != this->objectToModel.end())
      this->processModel(objectStorage, it->second, objectOffset);

    this->entityManager.registerObjectEntity(this->objectToEntity.at(obj.get()), this->total.orbital + objectOffset);

    if (it != this->objectToModel.end())
    {
      this->entityManager.registerModelEntity(this->objectToEntity.at(obj.get()), this->modelTotal.orbital + objectOffset);
      if (it->second->hasFlag(ModelFlags::Special))
      {
        this->entityManager.registerSpecialEntity(this->objectToEntity.at(obj.get()), this->total.orbital + objectOffset);
        this->trailManager.registerTrail(this->objectToEntity.at(obj.get()));
      }
    }

    objectOffset++;
  }

  for (size_t i = 0; i < this->orbitalObjects.size(); i++)
  {
    Object *central = this->orbitalObjects[i]->getOrbit()->getCentralBody();
    orbitalStorage.database.physics.centralBodyIndices[i] = this->findCentralBodyIndex(central);
  }

  std::atomic_size_t orbitalIndex{orbitalOffset};
  std::atomic_size_t objectIndex{objectOffset};
  for (std::unique_ptr<System> &sys : this->systems)
    this->processSystem(sys.get(), objectStorage, orbitalStorage, objectIndex, orbitalIndex);

  for (size_t i = 0; i < orbitalStorage.lookup.models.size(); i++)
  {
    Range range = instanceManager.reserve(orbitalStorage.lookup.models[i], orbitalStorage.modelCapacities[i]);
    orbitalStorage.database.render.modelRangeStart.push_back(range.begin);
    orbitalStorage.database.render.modelRangeEnd.push_back(range.end);
    orbitalStorage.database.render.models.push_back(orbitalStorage.lookup.models[i]);
  }

  for (size_t i = 0; i < objectStorage.lookup.models.size(); i++)
  {
    Range range = instanceManager.reserve(objectStorage.lookup.models[i], objectStorage.modelCapacities[i]);
    objectStorage.database.render.modelRangeStart.push_back(range.begin);
    objectStorage.database.render.modelRangeEnd.push_back(range.end);
    objectStorage.database.render.models.push_back(objectStorage.lookup.models[i]);
  }

  // orbital MUST be first
  orbitalStorage.database.combine(objectStorage.database);

  orbitalStorage.database.physics.loveNumbers = loveNumbers;
  orbitalStorage.database.physics.tidalFactors = tidalFactors;
  orbitalStorage.database.shared.total = this->total;

  return orbitalStorage.database;
}