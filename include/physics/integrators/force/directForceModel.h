#pragma once

#include "physics/integrators/force/forceModel.h"

#include <vector>

struct Entity;

template <typename Real>
class DirectForceModel : public ForceModel<Real>
{
private:
  using vec3 = typename RealTypes<Real>::vec3;

public:
  DirectForceModel(ThreadPool &threadPool);
  ~DirectForceModel();

  std::vector<typename RealTypes<Real>::vec3> calculateAccelerations(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) override;
  std::vector<typename RealTypes<Real>::vec3> calculateTorques(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) override;
};

#include "physics/integrators/force/directForceModel.hpp"