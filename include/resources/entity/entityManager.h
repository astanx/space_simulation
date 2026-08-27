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

public:
  EntityManager() = default;
  ~EntityManager() = default;

  const Entity &create();

  void registerOrbitalEntity(const Entity &entity, size_t idx);
  void registerObjectEntity(const Entity &entity, size_t idx);
  void registerModelEntity(const Entity &entity, size_t idx);

  std::vector<std::unique_ptr<Entity>> &getEntities();
  const std::vector<std::unique_ptr<Entity>> &getEntities() const;
  size_t getOrbitalIndex(const Entity &entity) const;
  size_t getObjectIndex(const Entity &entity) const;
  size_t getModelIndex(const Entity &entity) const;
  bool getIsOrbital(const Entity &entity) const;
};