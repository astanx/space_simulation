#pragma once

#include "debug/validators/validator.h"

#include "physics/world/data/physicsDatabaseView.h"

#include "resources/data/realTypes.h"

#include "resources/threadPool/threadPool.h"
#include "resources/entity/entity.h"
#include "resources/data/date.h"

#include <vector>
#include <unordered_map>

template <typename Real>
struct EnergyBodySample
{
  Date date;

  Real kineticEnergy = 0;
  Real rotationalEnergy = 0;

  Real kineticEnergyError = 0;
  Real rotationalEnergyError = 0;

  EnergyBodySample() = default;
  EnergyBodySample(Date date) : date(date) {};
};

template <typename Real>
struct EnergySystemSample
{
  Date date;

  Real potentialEnergy = 0;
  Real totalEnergy = 0;

  Real potentialEnergyError = 0;
  Real totalEnergyError = 0;

  EnergySystemSample() = default;
  EnergySystemSample(Date date) : date(date) {};
};

struct Entity;

template <typename Real, typename WorldReal>
class EnergyValidator : public Validator
{
private:
  using vec3 = typename RealTypes<Real>::vec3;
  using mat3 = typename RealTypes<Real>::mat3;
  using quat = typename RealTypes<Real>::quat;

  ThreadPool &threadPool;

  size_t historyIdx = 0;
  size_t steps = 0;
  std::vector<std::vector<EnergyBodySample<Real>>> bodyHistory;
  std::vector<EnergySystemSample<Real>> systemHistory;
  std::unordered_map<size_t, Entity> indexToEntity;

  Real calculateError(Real current, Real prev);

  void initIndices(const std::vector<Entity> &entities);

  void calculateEnergy(const PhysicsDatabaseView<WorldReal> &database, Date date);

public:
  EnergyValidator(ThreadPool &threadPool);
  ~EnergyValidator();

  void init(Scene &scene, Date date, size_t steps) override;
  void update(Scene &scene, Date date) override;
  void sendTable(Scene &scene) override;
  void saveTable(Scene &scene, const std::filesystem::path &folderPath) override;

  bool isFinished() override { return this->historyIdx >= this->steps; };
};

#include "debug/validators/energyValidator.hpp"