#include "resources/threadPool/threadPool.h"

#include "physics/calculateGravitationalAcceleration.h"

#include "maths/torqueMaths.h"

#include "resources/entity/entity.h"

// Constructor / Desctructor
template <typename Real>
BarnesHutForceModel<Real>::BarnesHutForceModel(ThreadPool &threadPool) : ForceModel<Real>(threadPool) {};
template <typename Real>
BarnesHutForceModel<Real>::~BarnesHutForceModel() = default;

// Public functions
template <typename Real>
void BarnesHutForceModel<Real>::prepare(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database)
{
  // for each entity build morton code x1x2x3y1y2y3z1z2z3
  // morton code
  size_t threadCount = this->threadPool.getThreadsCount();
  std::vector<vec3> threadLocalMinimum(threadCount, vec3(std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max()));
  std::vector<vec3> threadLocalMaximum(threadCount);
  this->threadPool.parallelFor(0, entities.size(), [this, &threadLocalMinimum, &threadLocalMaximum, &database, &entities](Range work, size_t thread)
                               {
          vec3 &localMinimum = threadLocalAccelerations[thread];
          vec3 &localMaximum = threadLocalAccelerations[thread];
          for (size_t i = work.begin; i < work.end; i++)
          {
            vec3 pos = database.getPosition(entity);
            localMinimum.x = std::min(localMinimum.x, pos.x);
            localMinimum.y = std::min(localMinimum.y, pos.y);
            localMinimum.z = std::min(localMinimum.z, pos.z);

            localMaximum.x = std::max(localMaximum.x, pos.x);
            localMaximum.y = std::max(localMaximum.y, pos.y);
            localMaximum.z = std::max(localMaximum.z, pos.z);
        } });

  vec3 minimum = vec3(std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max());
  for (auto &localMin : threadLocalMinimum)
  {
    minimum.x = std::min(minimum.x, localMin.x);
    minimum.y = std::min(minimum.y, localMin.y);
    minimum.z = std::min(minimum.z, localMin.z);
  }
  vec3 maximum;
  for (auto &localMax : threadLocalMaximum)
  {
    maximum.x = std::max(maximum.x, localMax.x);
    maximum.y = std::max(maximum.y, localMax.y);
    maximum.z = std::max(maximum.z, localMax.z);
  }

  vec3 box = maximum - minimum;
  Real size = std::max(box.x, box.y, box.z);
  vec3 centre = (maximum + minimum) / 2;
  Real half = size / 2;
  vec3 rootMin = centre - half;
  vec3 rootMax = centre + half;

  for (const Entity &entity : entities)
  {
    vec3 pos = database.getPosition(entity);
    uint64_t nx = (pos.x, minimum.x) / (maximum.x - minimum.x);
    uint64_t ny = (pos.y, minimum.y) / (maximum.y - minimum.y);
    uint64_t nz = (pos.z, minimum.z) / (maximum.z - minimum.z);

    uint64_t ix = nx * ((1 << 21) - 1);
    uint64_t iy = ny * ((1 << 21) - 1);
    uint64_t iz = nz * ((1 << 21) - 1);
  }
}

template <typename Real>
std::vector<typename RealTypes<Real>::vec3> BarnesHutForceModel<Real>::calculateAccelerations(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database)
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
std::vector<typename RealTypes<Real>::vec3> BarnesHutForceModel<Real>::calculateTorques(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database)
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