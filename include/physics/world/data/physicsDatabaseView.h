#pragma once

#include "scene/world/data/sharedDatabaseView.h"
#include "render/world/data/renderDatabase.h"

#include "compute/clBuffer.h"
#include "physics/structs/radii.h"

#include "resources/entity/entityManager.h"
#include "resources/transform.h"

class PointLight;
class Camera;

template <typename Real>
struct PhysicsDatabaseView
{
private:
  const EntityManager &entityManager;
  const PhysicsDatabase<Real> &physics;
  const SharedDatabaseView<Real> shared;

public:
  PhysicsDatabaseView(const EntityManager &entityManager, const PhysicsDatabase<Real> &physics, const SharedDatabaseView<Real> shared) : entityManager(entityManager), physics(physics), shared(shared) {};
  ~PhysicsDatabaseView() = default;

  // Entity
  const std::vector<Entity> &getEntities() const { return this->entityManager.getEntities(); };
  size_t getEntitiesCount() const { return this->entityManager.getEntities().size(); };

  // Shared
  Quat<Real> getOrientation(const Entity entity) const { return this->shared.getOrientation(this->entityManager.getObjectIndex(entity)); };
  Vec3<Real> getPosition(const Entity entity) const { return this->shared.getRealPosition(this->entityManager.getObjectIndex(entity)); };

  // Physics
  Vec3<Real> getAngularVelocity(const Entity entity) const { return this->physics.angularVelocities[this->entityManager.getObjectIndex(entity)]; };
  Vec3<Real> getVelocity(const Entity entity) const { return this->physics.velocities[this->entityManager.getObjectIndex(entity)]; };
  Mat3<Real> getInertiaTensor(const Entity entity) const { return this->physics.inertiaTensors[this->entityManager.getObjectIndex(entity)]; };
  Real getMu(const Entity entity) const { return this->physics.mus[this->entityManager.getObjectIndex(entity)]; };
};