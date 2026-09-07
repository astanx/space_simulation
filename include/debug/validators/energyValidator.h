#pragma once

#include "debug/validators/validator.h"

#include "physics/integrators/data/integratorDatabase.h"

#include "resources/realTypes.h"

#include <vector>

template <typename Real>
struct Sample
{
  Real elapsedTime;
  Real potentialEnergy;
  Real kineticEnergy;
  Real rotationalEnergy;
  Real totalEnergy;
};

struct Entity;

template <typename Real>
class EnergyValidator : public Validator
{
private:
  using vec3 = typename RealTypes<Real>::vec3;
  using mat3 = typename RealTypes<Real>::mat3;
  using quat = typename RealTypes<Real>::quat;

  size_t historyIdx;
  std::vector<std::vector<Sample<Real>>> history;

  void calculateEnergy(const std::vector<Entity> &entities, IntegratorDatabase<Real> &database, double elapsedTime);

public:
  EnergyValidator();
  ~EnergyValidator();

  void init(Scene &scene, double elapsedTime) override;
  void update(Scene &scene, double elapsedTime) override;
};

#include "debug/validators/energyValidator.hpp"