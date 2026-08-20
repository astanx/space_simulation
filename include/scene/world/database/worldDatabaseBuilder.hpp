#pragma once

#include "scene/world/worldObject.h"

#include "physics/structs/radii.h"
#include "physics/systems/system.h"
#include "physics/object.h"
#include "physics/orbitalObject.h"

#include "render/modelSource.h"
#include "render/renderSystem.h"
#include "render/instanceManager.h"

// Private functions
template <typename Real>
void WorldDatabaseBuilder<Real>::processObject(Object *obj, WorldDatabase<Real> &data, size_t i)
{
  data.shared.positions[i] = static_cast<Vec3<Real>>(obj->getPosition());
  data.shared.orientations[i] = static_cast<Quat<Real>>(obj->getOrientation());

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
void WorldDatabaseBuilder<Real>::processModelSource(TemporaryStorage<Real> &storage, ModelSource *modelSource, size_t i)
{
  if (!modelSource)
    Logger::logFatal("World GPU Builder", "ModelSource is null");

  this->processModel(storage, modelSource->getMainLayer(), i);
}

template <typename Real>
void WorldDatabaseBuilder<Real>::processSystem(WorldSystem &system, TemporaryStorage<Real> &objectStorage, TemporaryStorage<Real> &orbitalStorage, std::atomic_size_t &objectIndex, std::atomic_size_t &orbitalIndex)
{
  system.physics->forEachObject([this, &system, &objectStorage, &orbitalStorage, &orbitalIndex, &objectIndex](Object &obj, size_t i)
                                {
      OrbitalObject* orb = dynamic_cast<OrbitalObject*>(&obj);
      if (orb)
      {
        size_t idx = orbitalIndex.fetch_add(1);
        this->processOrbital(orb, orbitalStorage.database, idx);
        this->processModel(orbitalStorage, system.render->getModelFromObjectIndex(i), idx);

        Object* central = orb->getOrbit()->getCentralBody();
        orbitalStorage.database.physics.centralBodyIndices[idx] = this->findCentralBodyIndex(central);
      }
      else
      { 
        size_t idx = objectIndex.fetch_add(1);
        this->processObject(&obj, objectStorage.database, idx); 
        this->processModel(objectStorage, system.render->getModelFromObjectIndex(i), idx);
      } });
}

template <typename Real>
size_t WorldDatabaseBuilder<Real>::findCentralBodyIndex(Object *central)
{
  for (size_t j = 0; j < this->worldObjects.size(); j++)
    if (this->worldObjects[j].physics == central)
      return j + this->total.orbital;

  for (size_t j = 0; j < this->worldOrbitalObjects.size(); j++)
    if (this->worldOrbitalObjects[j].physics == central)
      return j;

  Logger::logFatal("World Database Builder", "Central body was not found");
  return 0;
}

// Public functions
template <typename Real>
Planet *WorldDatabaseBuilder<Real>::createPlanet(Model &model, double mu, Radii radii, Object *centralBody, const KeplerElements keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters, double g)
{
  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radii, e, tidalParameters, gravityField, g);

  planet->setAngularVelocity(r.calculateAngularVelocity());
  planet->setOrientation(r.calculateOrientation());

  model.setImportance(this->importance.planet);
  planet->addMainLayer(&model);

  Planet *ptr = planet.get();

  // if (planet->getUseTrail())
  //   this->render.addTrail(planet->generateTrail());

  this->total.orbital++;
  this->total.total++;

  this->worldOrbitalObjects.push_back({ptr, ptr});
  this->objects.push_back(std::move(planet));

  return ptr;
}

template <typename Real>
Star *WorldDatabaseBuilder<Real>::createStar(Model &model, double mu,
                                             Radii radii, double luminosity, const RotationalElements rotationalElements, double timeAfterJD2000, glm::dvec3 position, glm::dvec3 velocity)
{
  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Star> star = std::make_unique<Star>(mu, radii, luminosity, position, velocity);

  star->setAngularVelocity(r.calculateAngularVelocity());
  star->setOrientation(r.calculateOrientation());

  model.setImportance(this->importance.star);
  star->addMainLayer(&model);

  Star *ptr = star.get();

  this->total.object++;
  this->total.total++;

  this->worldObjects.push_back({ptr, ptr});
  this->objects.push_back(std::move(star));

  return ptr;
}

template <typename Real>
Moon *WorldDatabaseBuilder<Real>::createMoon(Model &model, double mu, Radii radii, Planet *centralBody, const KeplerElements &keplerElements, const RotationalElements rotationalElements, double timeAfterJD2000, GravityField gravityField, TidalParameters tidalParameters)
{
  KeplerElements e = keplerElements;
  e.calculateMeanMotion(centralBody->getMu());
  e.advanceMeanAnomaly(timeAfterJD2000);

  RotationalElements r = rotationalElements;
  r.advanceFromJD2000(timeAfterJD2000);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radii, e, tidalParameters, gravityField);

  moon->setAngularVelocity(r.calculateAngularVelocity());
  moon->setOrientation(r.calculateOrientation());

  model.setImportance(this->importance.moon);

  moon->addMainLayer(&model);
  if (moon->getUseTrail())
    this->render.addTrail(moon->generateTrail());

  Moon *ptr = moon.get();

  assert(centralBody && "[SimulationWorld] ASSERT: No central body for moon");

  this->total.orbital++;
  this->total.total++;

  this->worldOrbitalObjects.push_back({ptr, ptr});
  this->objects.push_back(std::move(moon));

  return ptr;
}

template <typename Real>
AsteroidSystem *WorldDatabaseBuilder<Real>::createAsteroidSystem(ResourceManager &resourceManager, ThreadPool &threadPool, Object *centralBody, unsigned amount, double innerEdge, double outerEdge, double timeAfterJD2000)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(resourceManager, centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            timeAfterJD2000, this->importance.asteroid, threadPool);
  AsteroidSystem *ptr = system.get();

  this->total.orbital += system->getTotalObjects();
  this->total.total += system->getTotalObjects();

  this->worldSystems.push_back({ptr, ptr});
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

  for (WorldOrbitalObject &obj : this->worldOrbitalObjects)
  {
    this->processOrbital(obj.physics, orbitalStorage.database, orbitalOffset);
    this->processModelSource(orbitalStorage, obj.render, orbitalOffset);
    orbitalOffset++;
  }

  for (WorldObject &obj : this->worldObjects)
  {
    this->processObject(obj.physics, objectStorage.database, objectOffset);
    this->processModelSource(objectStorage, obj.render, objectOffset);
    objectOffset++;
  }

  for (size_t i = 0; i < this->worldOrbitalObjects.size(); i++)
  {
    Object *central = this->worldOrbitalObjects[i].physics->getOrbit()->getCentralBody();
    orbitalStorage.database.physics.centralBodyIndices[i] = this->findCentralBodyIndex(central);
  }

  std::atomic_size_t orbitalIndex{orbitalOffset};
  std::atomic_size_t objectIndex{objectOffset};
  for (WorldSystem &sys : worldSystems)
    this->processSystem(sys, objectStorage, orbitalStorage, objectIndex, orbitalIndex);

  for (size_t i = 0; i < orbitalStorage.lookup.models.size(); i++)
  {
    Range range = instanceManager.reserve(orbitalStorage.lookup.models[i], orbitalStorage.modelCapacities[i]);
    orbitalStorage.database.render.modelRangeStart.push_back(range.begin);
    orbitalStorage.database.render.modelRangeEnd.push_back(range.end);
  }
  orbitalStorage.database.render.models = std::move(orbitalStorage.lookup.models);

  for (size_t i = 0; i < objectStorage.lookup.models.size(); i++)
  {
    Range range = instanceManager.reserve(objectStorage.lookup.models[i], objectStorage.modelCapacities[i]);
    objectStorage.database.render.modelRangeStart.push_back(range.begin);
    objectStorage.database.render.modelRangeEnd.push_back(range.end);
  }
  objectStorage.database.render.models = std::move(objectStorage.lookup.models);

  // orbital MUST be first
  orbitalStorage.database.combine(objectStorage.database);

  orbitalStorage.database.physics.loveNumbers = loveNumbers;
  orbitalStorage.database.physics.tidalFactors = tidalFactors;

  return orbitalStorage.database;
}