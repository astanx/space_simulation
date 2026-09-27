#pragma once

#include "physics/integrators/force/forceModel.h"

#include <vector>

template <typename Real>
struct AABB
{
  glm::vec<3, Real> min;
  glm::vec<3, Real> max;
};

template <typename Real>
struct Node
{
  glm::vec<3, Real> massCenter;
  Real totalMu;
  AABB bounds;

  int children[8];
};

struct Entity;

template <typename Real>
class BarnesHutForceModel : public ForceModel<Real>
{
private:
  using vec3 = typename RealTypes<Real>::vec3;

  std::vector<Node<Real>> octree;

public:
  BarnesHutForceModel(ThreadPool &threadPool);
  ~BarnesHutForceModel();

  void prepare(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) override;

  std::vector<typename RealTypes<Real>::vec3> calculateAccelerations(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) override;
  std::vector<typename RealTypes<Real>::vec3> calculateTorques(const std::vector<Entity> &entities, const IntegratorDatabase<Real> &database) override;
};

#include "physics/integrators/force/barnesHutForceModel.hpp"