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
struct DataBodySample
{
  Date date;

  glm::vec<3, Real> relativePosition;
  glm::vec<3, Real> relativeVelocity;

  DataBodySample() = default;
  DataBodySample(Date &date) : date(date), relativePosition(glm::vec<3, Real>(0.0)), relativeVelocity(glm::vec<3, Real>(0.0)) {};
};

struct Entity;

template <typename Real, typename WorldReal>
class DataValidator : public Validator
{
private:
  using vec3 = typename RealTypes<Real>::vec3;

  ThreadPool &threadPool;

  const EntityManager *entityManager = nullptr;

  size_t historyIdx = 0;
  size_t steps = 0;
  std::vector<std::vector<DataBodySample<Real>>> bodyHistory;
  std::unordered_map<size_t, Entity> indexToEntity;

  void initIndices(const std::vector<Entity> &entities);
  void saveData(const PhysicsDatabaseView<WorldReal> &database, Date date);

public:
  DataValidator(ThreadPool &threadPool);
  ~DataValidator();

  void init(Scene &scene, Date date, size_t steps) override;
  void update(Scene &scene, Date date) override;
  void sendTable(Scene &scene) override;
  void saveTable(Scene &scene, const std::filesystem::path &folderPath) override;

  bool isFinished() override { return this->historyIdx >= this->steps; };
};

#include "debug/validators/dataValidator.hpp"