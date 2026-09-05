#pragma once

#include "physics/integrators/integratable.h"

#include "physics/systems/system.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/constants.h"
#include "physics/calculateGravitationalAcceleration.h"

#include "render/modelSource.h"

#include "maths/constants.h"
#include "maths/orbitalMaths.h"
#include "maths/torqueMaths.h"

#include "resources/threadPool.h"

#include <iostream>
#include <cmath>

// Private functions
template <typename Real>
bool WisdomHolmanIntegratorCPU<Real>::validEntity(const std::unique_ptr<Entity> &entity)
{
  if (!entity)
  {
    Logger::logError("Wisdom Holman Integrator CPU", "Uninitialized entity detected");
    return false;
  }
  return true;
}

template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::halfKickLinear(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt)
{
  std::vector<std::vector<vec3>> threadLocalAccelerations;
  threadLocalAccelerations.resize(this->threadPool.getThreadCount());
  for (auto &local : threadLocalAccelerations)
    local.resize(entities.size());

  this->threadPool.parallelFor(0, entities.size(), [this, &threadLocalAccelerations, &database, &entities](Range work, size_t thread)
                               {
    std::vector<vec3> &localAccelerations = threadLocalAccelerations[thread];
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity& entity = entities[i];

      size_t central = database.getIsOrbital(entity) ? database.getCentralBodyIdx(entity) : i;
      
      for (size_t j = i; j < entities.size(); j++)
      {
        vec3 acceleration = vec3(0.0);

        if (i == j)
          continue;
        if (j == central)
          continue;

        const Entity& otherEntity = entities[j];
        vec3 scale = gravitationalDpOverD3<Real>(database.getPosition(entity), database.getPosition(otherEntity));

        localAccelerations[i] += scale * database.getMu(otherEntity);
        localAccelerations[j] -= scale * database.getMu(entity);
      }
    } });

  this->threadPool.parallelFor(0, entities.size(), [this, &threadLocalAccelerations, &database, &entities, dt](Range work)
                               {
  for(size_t i = work.begin; i < work.end; i++)
  {
    const Entity& entity = entities[i];

    vec3 acceleration = vec3(0.0);
    for (auto& localAccelerations : threadLocalAccelerations)
      acceleration += localAccelerations[i];

    database.setVelocity(entity, vec3(database.getVelocity(entity)) + dt * acceleration); // kick
  } });
}

template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::halfKickAngular(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt)
{
  std::vector<std::vector<vec3>> threadLocalTorques;
  threadLocalTorques.resize(this->threadPool.getThreadCount());

  for (auto &local : threadLocalTorques)
    local.resize(entities.size());
  this->threadPool.parallelFor(0, entities.size(), [this, &threadLocalTorques, &database, &entities](Range work, size_t thread)
                               {
    std::vector<vec3> &localTorque = threadLocalTorques[thread];
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity& entity = entities[i];

      for (size_t j = i; j < entities.size(); j++)
      {
        if (i == j)
          continue;

        const Entity& otherEntity = entities[j];

        vec3 dp = vec3(database.getPosition(otherEntity)) -  vec3(database.getPosition(entity));
        Real d = glm::length(dp);

        {
          vec3 gravitationalTorque = ::calculateGravitationalTorque<Real>(dp, d, database.getQuadrupoleTensor(entity), database.getMu(otherEntity));

          TidalParameters p = database.getTidalParameters(entity);
          vec3 tidalTorque = vec3(0.0);
          if (p.k2 != -1 && p.Q != -1)
            tidalTorque = ::calculateTidalTorque<Real>(-dp, d, database.getAngularVelocity(entity), database.getVelocity(entity), database.getMeanRadius(entity), p.k2, p.Q, database.getVelocity(otherEntity), database.getMu(otherEntity));
          localTorque[i] += gravitationalTorque + tidalTorque;
        }

        {
          vec3 gravitationalTorque = ::calculateGravitationalTorque<Real>(-dp, d, database.getQuadrupoleTensor(otherEntity), database.getMu(entity));
          TidalParameters p = database.getTidalParameters(otherEntity);
          vec3 tidalTorque = vec3(0.0);
          if (p.k2 != -1 && p.Q != -1)
            tidalTorque = ::calculateTidalTorque<Real>(dp, d, database.getAngularVelocity(otherEntity), database.getVelocity(otherEntity), database.getMeanRadius(otherEntity), p.k2, p.Q, database.getVelocity(entity), database.getMu(entity));
        
          localTorque[j] += gravitationalTorque + tidalTorque;
        }
      }
    } });

  this->threadPool.parallelFor(0, entities.size(), [this, &threadLocalTorques, &database, &entities, dt](Range work)
                               {
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity &entity = entities[i];

      vec3 torque = vec3(0.0);
      for (auto &localTorques : threadLocalTorques)
        torque += localTorques[i];

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
    } });
}

template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::keplerDrift(const Entity &entity, IntegratorDatabase<Real> &database, Real dt)
{
  KeplerElements keplerElements = database.getKeplerElements(entity);
  size_t centralBodyIdx = database.getCentralBodyIdx(entity);
  keplerElements.advanceMeanAnomaly(dt);

  Real E = OrbitalMaths::calculateEccentricAnomaly(keplerElements.m, keplerElements.e);

  vec3 pos(0.0);

  pos.x = keplerElements.a * (cos(E) - keplerElements.e);
  pos.y = keplerElements.a * sqrt(1 - (keplerElements.e * keplerElements.e)) * sin(E);

  vec3 v(0.0);
  Real r = keplerElements.a * (1 - keplerElements.e * cos(E));

  v.x = -sqrt(database.getMu(centralBodyIdx) * keplerElements.a) / r * sin(E);
  v.y = sqrt(database.getMu(centralBodyIdx) * keplerElements.a * (1 - (keplerElements.e * keplerElements.e))) / r * cos(E);

  mat3 R = OrbitalMaths::createR3matrix<Real>(keplerElements.Omega) * OrbitalMaths::createR1matrix<Real>(keplerElements.i) * OrbitalMaths::createR3matrix<Real>(keplerElements.omega);

  database.setVelocity(entity, R * v + vec3(database.getVelocity(centralBodyIdx)));
  database.setPosition(entity, R * pos + vec3(database.getPosition(centralBodyIdx)));
  database.setMeanAnomaly(entity, keplerElements.m);
}

template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::driftLinear(const Entity &entity, IntegratorDatabase<Real> &database, Real dt)
{
  if (database.getIsOrbital(entity))
    this->keplerDrift(entity, database, dt);
  else
    database.setPosition(entity, vec3(database.getPosition(entity)) + vec3(database.getVelocity(entity)) * dt);
}

template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::driftAngular(const Entity &entity, IntegratorDatabase<Real> &database, Real dt)
{
  vec3 omega = database.getAngularVelocity(entity);
  Real omega_len = glm::length(omega);
  Real theta = omega_len * dt;
  if (std::fabs(theta) > EPS)
  {
    vec3 axis = omega / omega_len;

    Real half = theta * 0.5;
    quat q_rot(cos(half), sin(half) * axis.x, sin(half) * axis.y, sin(half) * axis.z);
    database.setOrientation(entity, glm::normalize(q_rot * quat(database.getOrientation(entity))));
  }
}

template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::drift(const Entity &entity, IntegratorDatabase<Real> &database, Real dt)
{
  this->driftLinear(entity, database, dt);
  this->driftAngular(entity, database, dt);
}
template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::halfKick(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, Real dt)
{
  this->halfKickLinear(entities, database, dt);
  this->halfKickAngular(entities, database, dt);
}

// Public functions
template <typename Real>
void WisdomHolmanIntegratorCPU<Real>::step(IntegratorDatabase<Real> &database, Real dt)
{
  const std::vector<Entity> &entities = database.getEntities();

  // std::cout << std::setprecision(17) << "FIRST ENERGY: " << this->calculateEnergy(entities, database) << std::endl;
  // kick
  this->halfKick(entities, database, dt * 0.5);

  // drift
  for (const Entity &entity : entities)
    this->drift(entity, database, dt);

  // kick
  this->halfKick(entities, database, dt * 0.5);

  // std::cout << std::setprecision(17) << "AFTER ENERGY: " << this->calculateEnergy(entities, database) << std::endl;
}