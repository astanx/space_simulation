#pragma once

#include "physics/integrators/data/integratorDatabase.h"

#include "resources/data/realTypes.h"

#include <vector>

struct Entity;

template <typename Real>
class ForceModel
{
protected:
  ThreadPool &threadPool;

public:
  ForceModel(ThreadPool &threadPool) : threadPool(threadPool) {};
  virtual ~ForceModel() = default;

  virtual std::vector<typename RealTypes<Real>::vec3> calculateAccelerations(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) = 0;
  virtual std::vector<typename RealTypes<Real>::vec3> calculateTorques(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) = 0;
};