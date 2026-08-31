#pragma once

#include "debug/logger.h"

#include "resources/entity/entity.h"

#include <vector>
#include <unordered_map>
#include <memory>

class EntityManager
{
private:
  std::vector<std::unique_ptr<Entity>> entities;
  std::unordered_map<size_t, size_t> entityToOrbitalIdx;
  std::unordered_map<size_t, size_t> entityToObjectIdx;
  std::unordered_map<size_t, size_t> entityToModelIdx;
  std::unordered_map<size_t, size_t> entityToSpecialIdx;

  // mb make it own a copy of threadPool and call forEach with it sometimes

public:
  EntityManager() = default;
  ~EntityManager() = default;

  const Entity &create();

  template <typename F>
  void forEachSpecialEntity(F &&func) const;

  void registerOrbitalEntity(const Entity &entity, size_t idx);
  void registerObjectEntity(const Entity &entity, size_t idx);
  void registerModelEntity(const Entity &entity, size_t idx);
  void registerSpecialEntity(const Entity &entity, size_t idx);

  const std::vector<std::unique_ptr<Entity>> &getEntities() const;
  size_t getOrbitalIndex(const Entity &entity) const;
  size_t getObjectIndex(const Entity &entity) const;
  size_t getModelIndex(const Entity &entity) const;
  bool getIsOrbital(const Entity &entity) const;
};

#include "resources/entity/entityManager.hpp"