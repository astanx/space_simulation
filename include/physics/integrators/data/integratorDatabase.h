#pragma once

#include "physics/integrators/data/integratorDatabase.h"

#include "physics/structs/keplerElements.h"
#include "physics/structs/tidalParameters.h"

#include "scene/world/data/sharedDatabase.h"
#include "physics/world/data/physicsDatabase.h"

#include "resources/entity/entityManager.h"

template <typename Real>
struct IntegratorDatabase
{
private:
  const EntityManager &entityManager;

  SharedDatabase<Real> &shared;
  PhysicsDatabase<Real> &physics;

public:
  IntegratorDatabase(const EntityManager &manager, SharedDatabase<Real> &shared, PhysicsDatabase<Real> &physics) : entityManager(manager), shared(shared), physics(physics) {};
  virtual ~IntegratorDatabase() = default;

  const std::vector<Entity> &getEntities() { return entityManager.getEntities(); };

  Vec3<Real> getPosition(const Entity &entity) const { return this->shared.positions[this->entityManager.getObjectIndex(entity)]; };
  Vec3<Real> getPosition(size_t idx) const { return this->shared.positions[idx]; };
  real getMeanRadius(const Entity &entity) const { return this->shared.meanRadii[this->entityManager.getObjectIndex(entity)]; };

  size_t getCentralBodyIdx(const Entity &entity) const { return this->physics.centralBodyIndices[this->entityManager.getOrbitalIndex(entity)]; };
  bool getIsOrbital(const Entity &entity) const { return this->entityManager.getIsOrbital(entity); };
  Vec3<Real> getVelocity(const Entity &entity) const { return this->physics.velocities[this->entityManager.getObjectIndex(entity)]; };
  Vec3<Real> getVelocity(size_t idx) const { return this->physics.velocities[idx]; };
  Vec3<Real> getAngularVelocity(const Entity &entity) const { return this->physics.angularVelocities[this->entityManager.getObjectIndex(entity)]; };
  Mat3<Real> getInertiaTensor(const Entity &entity) const { return this->physics.inertiaTensors[this->entityManager.getObjectIndex(entity)]; };
  Mat3<Real> getQuadrupoleTensor(const Entity &entity) const { return this->physics.quadrupoleTensors[this->entityManager.getObjectIndex(entity)]; };
  KeplerElements<Real> getKeplerElements(const Entity &entity) const
  {
    size_t idx = this->entityManager.getOrbitalIndex(entity);
    KeplerElements<Real> e;
    e.a = this->physics.semiAxises[idx];
    e.e = this->physics.eccentricities[idx];
    e.i = this->physics.inclinations[idx];
    e.Omega = this->physics.longitude[idx];
    e.omega = this->physics.periapsis[idx];
    e.m = this->physics.meanAnomaly[idx];
    e.n = this->physics.meanMotion[idx];

    return e;
  };
  TidalParameters getTidalParameters(const Entity &entity) const
  {
    TidalParameters params;
    size_t idx = this->entityManager.getObjectIndex(entity);
    int loveIdx = this->physics.loveIndices[idx];
    int tidalIdx = this->physics.tidalFactorIndices[idx];
    if (loveIdx == -1 || tidalIdx == -1)
    {
      params.k2 = -1;
      params.Q = -1;
    }
    else
    {
      params.k2 = this->physics.loveNumbers[loveIdx];
      params.Q = this->physics.tidalFactors[loveIdx];
    }

    return params;
  };
  Real getMu(const Entity &entity) { return this->physics.mus[this->entityManager.getObjectIndex(entity)]; };
  Real getMu(size_t idx) { return this->physics.mus[idx]; };

  Quat<Real> getOrientation(const Entity &entity) { return this->shared.orientations[this->entityManager.getObjectIndex(entity)]; };
  void setPosition(const Entity &entity, Vec3<Real> pos) { this->shared.positions[this->entityManager.getObjectIndex(entity)] = pos; };
  void setOrientation(const Entity &entity, Quat<Real> orientation) { this->shared.orientations[this->entityManager.getObjectIndex(entity)] = orientation; };

  void setAngularVelocity(const Entity &entity, Vec3<Real> omega) { this->physics.angularVelocities[this->entityManager.getObjectIndex(entity)] = omega; };
  void setVelocity(const Entity &entity, Vec3<Real> vel) { this->physics.velocities[this->entityManager.getObjectIndex(entity)] = vel; };
  void setMeanAnomaly(const Entity &entity, Real anomaly) { this->physics.meanAnomaly[this->entityManager.getOrbitalIndex(entity)] = anomaly; };
};