#include "resources/threadPool/threadPool.h"

#include "physics/calculateGravitationalAcceleration.h"

#include "maths/torqueMaths.h"

#include "resources/entity/entity.h"

// Constructor / Desctructor
template <typename Real>
DirectForceModel<Real>::DirectForceModel(ThreadPool &threadPool) : ForceModel<Real>(threadPool) {};
template <typename Real>
DirectForceModel<Real>::~DirectForceModel() = default;

// Public functions
template <typename Real>
std::vector<typename RealTypes<Real>::vec3> DirectForceModel<Real>::calculateAccelerations(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database)
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

  std::vector<vec3> accelerations;
  accelerations.resize(entities.size());

  for (auto &local : threadLocalAccelerations)
    for (size_t i = 0; i < entities.size(); i++)
      accelerations[i] += local[i];

  return accelerations;
}

template <typename Real>
std::vector<typename RealTypes<Real>::vec3> DirectForceModel<Real>::calculateTorques(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database)
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

        vec3 dp = vec3(database.getPosition(otherEntity)) - vec3(database.getPosition(entity));
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

  std::vector<vec3> torques;
  torques.resize(entities.size());

  for (auto &local : threadLocalTorques)
    for (size_t i = 0; i < entities.size(); i++)
      torques[i] += local[i];

  return torques;
}