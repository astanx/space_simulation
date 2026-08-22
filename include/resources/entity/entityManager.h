#pragma once

#include "debug/logger.h"

#include "resources/entity/entity.h"

#include <vector>
#include <unordered_map>

class EntityManager
{
private:
  std::vector<Entity> entities;
  std::unordered_map<const Entity *, size_t> entityToOrbitalIdx;
  std::unordered_map<const Entity *, size_t> entityToObjectIdx;
  std::unordered_map<const Entity *, size_t> entityToModelIdx;

public:
  EntityManager() = default;
  ~EntityManager() = default;

  const Entity &create();

  void registerOrbitalEntity(const Entity &entity, size_t idx);
  void registerObjectEntity(const Entity &entity, size_t idx);
  void registerModelEntity(const Entity &entity, size_t idx);

  const std::vector<Entity> &getEntities() const;
  size_t getOrbitalIndex(const Entity &entity) const;
  size_t getObjectIndex(const Entity &entity) const;
  size_t getModelIndex(const Entity &entity) const;
};