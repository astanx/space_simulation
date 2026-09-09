#pragma once

#include "debug/validators/validator.h"

#include "physics/world/data/physicsDatabaseView.h"

#include "resources/data/realTypes.h"

#include "resources/threadPool.h"

#include <vector>

template <typename Real>
struct BodySample
{
  Real elapsedTime = 0;

  Real kineticEnergy = 0;
  Real rotationalEnergy = 0;

  Real kineticEnergyError = 0;
  Real rotationalEnergyError = 0;
};

template <typename Real>
struct SystemSample
{
  Real elapsedTime = 0;

  Real potentialEnergy = 0;
  Real totalEnergy = 0;

  Real potentialEnergyError = 0;
  Real totalEnergyError = 0;
};

struct Entity;

template <typename Real>
class EnergyValidator : public Validator
{
private:
  using vec3 = typename RealTypes<Real>::vec3;
  using mat3 = typename RealTypes<Real>::mat3;
  using quat = typename RealTypes<Real>::quat;

  ThreadPool &threadPool;

  size_t historyIdx = 0;
  size_t steps = 0;
  std::vector<std::vector<BodySample<Real>>> bodyHistory;
  std::vector<SystemSample<Real>> systemHistory;

  Real calculateError(Real current, Real prev);

  void calculateEnergy(const PhysicsDatabaseView<Real> &database, double elapsedTime);

public:
  EnergyValidator(ThreadPool &threadPool);
  ~EnergyValidator();

  void init(Scene &scene, double elapsedTime, size_t steps) override;
  void update(Scene &scene, double elapsedTime) override;
};

#include "debug/validators/energyValidator.hpp"