#pragma once

#include "resources/entity/entity.h"

#include <glm/glm.hpp>

// Private functions
template <typename Real>
void EnergyValidator<Real>::calculateEnergy(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, double elapsedTime)
{
  this->threadPool.parallelFor(0, entities.size(), [this, &database, &entities, elapsedTime](Range work)
                               {
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity &entity = entities[i];

      Real kineticEnergy = 0.5 * database.getMu(entity) / G * glm::dot(vec3(database.getVelocity(entity)), vec3(database.getVelocity(entity)));
      quat q = database.getOrientation(entity);
      mat3 R = glm::mat3_cast(q);
      vec3 omega = glm::transpose(R) * database.getAngularVelocity(entity);
      mat3 tensor = database.getInertiaTensor(entity);

      Real rotationalEnergy = 0.5 * glm::dot(omega, tensor * omega);

      Real potentialEnergy = 0;
      for (size_t j = i + 1; j < entities.size(); j++)
      {
        const Entity &otherEntity = entities[j];
        potentialEnergy -= database.getMu(entity) * database.getMu(otherEntity) / G  / glm::length(vec3(database.getPosition(entity)) - vec3(database.getPosition(otherEntity)));
      }
      this->history[this->historyIdx][i] = Sample<Real>{elapsedTime, potentialEnergy, kineticEnergy, rotationalEnergy, potentialEnergy + kineticEnergy + rotationalEnergy};
    } });
}

// Constructor / Destructor
template <typename Real>
EnergyValidator<Real>::EnergyValidator() = default;
template <typename Real>
EnergyValidator<Real>::~EnergyValidator() = default;

// Public functions
template <typename Real>
void EnergyValidator<Real>::init(Scene &scene, double elapsedTime)
{
  this->update(scene, elapsedTime);
}
template <typename Real>
void EnergyValidator<Real>::update(Scene &scene, double elapsedTime)
{
}
