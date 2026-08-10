#pragma once

#include "scene/world/worldGPUBuilder.h"

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
void WorldGPUBuilder<Real>::processObject(Object *obj, WorldDataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex)
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
    std::lock_guard<std::mutex> lock(loveMutex);

    loveNumbers.push_back(static_cast<Real>(p.k2));
    data.physics.loveIndices[i] = loveNumbers.size() - 1;
  }
  if (p.Q != -1)
  {
    std::lock_guard<std::mutex> lock(tidalMutex);

    tidalFactors.push_back(static_cast<Real>(p.Q));
    data.physics.tidalFactorIndices[i] = tidalFactors.size() - 1;
  }
};

template <typename Real>
void WorldGPUBuilder<Real>::processOrbital(OrbitalObject *obj, WorldDataGPU<Real> &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex)
{
  this->processObject(obj, data, i, loveNumbers, tidalFactors, loveMutex, tidalMutex);

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
void WorldGPUBuilder<Real>::processModel(std::vector<size_t> &modelCapacities, LookupTable &lookup, Model *model, WorldDataGPU<Real> &data, size_t i, std::mutex &modelMutex)
{
  if (!model)
    Logger::logFatal("World GPU Builder", "Model is null");

  {
    std::lock_guard<std::mutex> lock(modelMutex);
    size_t modelIndex;

    auto it = lookup.table.find(model);
    if (it == lookup.table.end())
    {
      modelIndex = lookup.freeIndex++;

      lookup.table[model] = modelIndex;
      lookup.models.push_back(model);
      data.render.isNonFullable.push_back(model->hasFlag(ModelFlags::None) ? 0 : 1);
      data.render.modelImportances.push_back(model->getImportance());
      data.render.modelColors.push_back(model->getAverageColor());
      data.render.modelTextureLayers.push_back(model->getImpostorLayer());
      modelCapacities.push_back(0);
    }
    else
      modelIndex = it->second;

    modelCapacities[modelIndex]++;
  }
}

template <typename Real>
void WorldGPUBuilder<Real>::processModelSource(std::vector<size_t> &modelCapacities, LookupTable &lookup, ModelSource *modelSource, WorldDataGPU<Real> &data, size_t i, std::mutex &modelMutex)
{
  if (!modelSource)
    Logger::logFatal("World GPU Builder", "ModelSource is null");

  this->processModel(modelCapacities, lookup, modelSource->getMainLayer(), data, i, modelMutex);
}

// Public functions
template <typename Real>
WorldDataGPU<Real> WorldGPUBuilder<Real>::build(std::vector<WorldObject> &worldObjects, std::vector<WorldSystem> &worldSystems, InstanceManager &instanceManager)
{
  std::vector<WorldOrbitalObject> orbitalObjects;
  std::vector<WorldObject> objects;
  Total total;

  for (WorldObject object : worldObjects)
  {
    OrbitalObject *orb = dynamic_cast<OrbitalObject *>(object.physics);
    if (orb)
      orbitalObjects.push_back({orb, object.render});
    else
      objects.push_back(object);
  }

  total.orbital = orbitalObjects.size();

  for (WorldSystem sys : worldSystems)
  {
    std::atomic_size_t sysOrbital = 0;

    sys.physics->forEachObject([&sysOrbital](Object &obj, size_t i)
                               { 
                        if (dynamic_cast<OrbitalObject *>(&obj) != nullptr) 
                          sysOrbital.fetch_add(1); });

    total.orbital += sysOrbital;
  }

  total.total = objects.size() + orbitalObjects.size();

  for (WorldSystem sys : worldSystems)
    total.total += sys.physics->getTotalObjects();

  WorldDataGPU<Real> objectGPU;
  WorldDataGPU<Real> orbitalGPU;
  LookupTable objectLookup;
  LookupTable orbitalLookup;
  std::vector<size_t> objectModelCapacities;
  std::vector<size_t> orbitalModelCapacities;

  total.object = total.total - total.orbital;
  objectGPU.resize(total.object);
  orbitalGPU.resize(total.orbital);

  size_t orbitalOffset = 0;
  size_t objectOffset = 0;

  std::mutex loveMutex;
  std::mutex tidalMutex;
  std::mutex modelMutex;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;

  for (WorldOrbitalObject &obj : orbitalObjects)
  {
    this->processOrbital(obj.physics, orbitalGPU, orbitalOffset, loveNumbers, tidalFactors, loveMutex, tidalMutex);
    this->processModelSource(orbitalModelCapacities, orbitalLookup, obj.render, orbitalGPU, orbitalOffset, modelMutex);
    orbitalOffset++;
  }

  for (WorldObject &obj : objects)
  {
    this->processObject(obj.physics, objectGPU, objectOffset, loveNumbers, tidalFactors, loveMutex, tidalMutex);
    this->processModelSource(objectModelCapacities, objectLookup, obj.render, objectGPU, objectOffset, modelMutex);
    objectOffset++;
  }

  for (size_t i = 0; i < orbitalObjects.size(); i++)
  {
    Object *central = orbitalObjects[i].physics->getOrbit()->getCentralBody();
    for (size_t j = 0; j < objects.size(); j++)
    {
      if (objects[j].physics == central)
        orbitalGPU.physics.centralBodyIndices[i] = j + total.orbital;
    }

    for (size_t j = 0; j < orbitalObjects.size(); j++)
    {
      if (orbitalObjects[j].physics == central)
        orbitalGPU.physics.centralBodyIndices[i] = j;
    }
  }

  std::atomic_size_t orbitalIndex{orbitalOffset};
  std::atomic_size_t objectIndex{objectOffset};
  for (WorldSystem &sys : worldSystems)
    sys.physics->forEachObject([this, &sys, &orbitalGPU, &objectGPU, &orbitalLookup, &objectLookup, &orbitalModelCapacities, &objectModelCapacities, &orbitalIndex, &objectIndex, &loveNumbers, &tidalFactors, &loveMutex, &tidalMutex, &modelMutex, &objects, &orbitalObjects, &total](Object &obj, size_t i)
                               {
      OrbitalObject* orb = dynamic_cast<OrbitalObject*>(&obj);
      if (orb)
      {
        size_t idx = orbitalIndex.fetch_add(1);
        this->processOrbital(orb, orbitalGPU, idx, loveNumbers, tidalFactors, loveMutex, tidalMutex);
        this->processModel(orbitalModelCapacities, orbitalLookup, sys.render->getModelFromObjectIndex(i), orbitalGPU, idx, modelMutex);

        Object* central = orb->getOrbit()->getCentralBody();
        for (size_t j = 0; j < objects.size(); j++)
        {
          if (objects[j].physics == central)
            orbitalGPU.physics.centralBodyIndices[idx] = j + total.orbital;
        }

        for (size_t j = 0; j < orbitalObjects.size(); j++)
        {
          if (orbitalObjects[j].physics == central)
            orbitalGPU.physics.centralBodyIndices[idx] = j;
        }
      }
      else
      { 
        size_t idx = objectIndex.fetch_add(1);
        this->processObject(&obj, objectGPU, idx, loveNumbers, tidalFactors, loveMutex, tidalMutex); 
        this->processModel(objectModelCapacities, objectLookup, sys.render->getModelFromObjectIndex(i), objectGPU, idx, modelMutex);
      } });

  for (size_t i = 0; i < orbitalLookup.models.size(); i++)
  {
    Range range = instanceManager.reserve(orbitalLookup.models[i], orbitalModelCapacities[i]);
    orbitalGPU.render.modelRangeStart.push_back(range.begin);
    orbitalGPU.render.modelRangeEnd.push_back(range.end);
  }
  orbitalGPU.render.models = std::move(orbitalLookup.models);

  for (size_t i = 0; i < objectLookup.models.size(); i++)
  {
    Range range = instanceManager.reserve(objectLookup.models[i], objectModelCapacities[i]);
    objectGPU.render.modelRangeStart.push_back(range.begin);
    objectGPU.render.modelRangeEnd.push_back(range.end);
  }
  objectGPU.render.models = std::move(objectLookup.models);

  // orbital MUST be first
  orbitalGPU.combine(objectGPU);

  orbitalGPU.physics.loveNumbers = loveNumbers;
  orbitalGPU.physics.tidalFactors = tidalFactors;

  orbitalGPU.total = total;

  return orbitalGPU;
}