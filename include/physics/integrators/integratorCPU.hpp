#pragma once

#include "physics/integrators/data/integratorDatabase.h"

#include <glm/glm.hpp>

template <typename Real>
Real IntegratorCPU<Real>::calculateEnergy(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database)
{
  std::vector<Real> threadLocalEnergy;
  threadLocalEnergy.resize(this->threadPool.getThreadCount(), Real(0));
  this->threadPool.parallelFor(0, entities.size(), [this, &threadLocalEnergy, &database, &entities](Range work, size_t thread)
                               {
    Real &localEnergy = threadLocalEnergy[thread];
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity &entity = entities[i];

      localEnergy += 0.5 * database.getMu(entity) / G * glm::dot(vec3(database.getVelocity(entity)), vec3(database.getVelocity(entity)));
      quat q = database.getOrientation(entity);
      mat3 R = glm::mat3_cast(q);
      vec3 omega = glm::transpose(R) * database.getAngularVelocity(entity);
      mat3 tensor = database.getInertiaTensor(entity);

      localEnergy += 0.5 * glm::dot(omega, tensor * omega);
      for (size_t j = i; j < entities.size(); j++)
      {
      const Entity &otherEntity = entities[j];
        if (i == j) continue;
        localEnergy -= database.getMu(entity) * database.getMu(otherEntity) / G  / glm::length(vec3(database.getPosition(entity)) - vec3(database.getPosition(otherEntity)));
      }
    } });
  Real total = 0;
  for (auto energy : threadLocalEnergy)
    total += energy;

  return total;
}