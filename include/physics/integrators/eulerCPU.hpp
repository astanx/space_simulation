#pragma once

#include "physics/integrators/integratable.h"

#include "physics/systems/system.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/constants/constants.h"
#include "physics/calculateGravitationalAcceleration.h"

#include "maths/constants.h"
#include "maths/orbitalMaths.h"
#include "maths/torqueMaths.h"

#include "resources/threadPool/threadPool.h"

#include <iostream>
#include <cmath>

// Private functions
template <typename Real>
void EulerIntegratorCPU<Real>::driftLinear(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt)
{
  std::vector<vec3> accelerations = this->forceModel->calculateAccelerations(entities, database);
  this->threadPool.parallelFor(0, entities.size(), [this, &accelerations, &database, &entities, dt](Range work)
                               {
  for(size_t i = work.begin; i < work.end; i++)
  {
    const Entity& entity = entities[i];
    database.setVelocity(entity, vec3(database.getVelocity(entity)) + dt * accelerations[i]); // kick
    database.setPosition(entity, vec3(database.getPosition(entity)) + vec3(database.getVelocity(entity)) * dt); // drift
  } });
}

template <typename Real>
void EulerIntegratorCPU<Real>::driftAngular(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt)
{
  std::vector<vec3> torques = this->forceModel->calculateTorques(entities, database);
  this->threadPool.parallelFor(0, entities.size(), [this, &torques, &database, &entities, dt](Range work)
                               {
  for(size_t i = work.begin; i < work.end; i++)
  {
    const Entity &entity = entities[i];

    vec3 torque = torques[i];

    mat3 tensor = database.getInertiaTensor(entity);
    Real det = glm::determinant(tensor);

    if (std::fabs(det) < EPS || !std::isfinite(det))
      continue;

    quat q = database.getOrientation(entity);

    mat3 R = glm::mat3_cast(q);
    mat3 transR = glm::transpose(R);
    vec3 omega = transR * database.getAngularVelocity(entity);
    torque = transR * torque;

    vec3 acc = glm::inverse(tensor) * (torque - cross(omega, tensor * omega));

    database.setAngularVelocity(entity, R * (omega + acc * dt));
    omega = database.getAngularVelocity(entity);
    Real omega_len = glm::length(omega);
    Real theta = omega_len * dt;
    if (std::fabs(theta) > EPS)
    {
      vec3 axis = omega / omega_len;

      Real half = theta * 0.5;
      quat q_rot(cos(half), sin(half) * axis.x, sin(half) * axis.y, sin(half) * axis.z);
      database.setOrientation(entity, glm::normalize(q_rot * quat(database.getOrientation(entity))));
    }
  } });
}

template <typename Real>
void EulerIntegratorCPU<Real>::drift(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt)
{
  this->driftLinear(entities, database, dt);
  this->driftAngular(entities, database, dt);
}

// Public functions
template <typename Real>
void EulerIntegratorCPU<Real>::step(IntegratorDatabase<Real> &database, Real dt)
{
  const std::vector<Entity> &entities = database.getEntities();

  this->drift(entities, database, dt);
}