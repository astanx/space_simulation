#include "resources/entity/entityManager.h"

#include "debug/logger.h"

#include <iostream>

// Public functions
const Entity &EntityManager::create()
{
  this->entities.push_back(std::make_unique<Entity>(this->entities.size()));
  return *this->entities.back();
};

void EntityManager::registerOrbitalEntity(const Entity &entity, size_t idx)
{
  auto it = this->entityToOrbitalIdx.find(entity.id);
  if (it != this->entityToOrbitalIdx.end())
    Logger::logFatal("Entity Manager", "Orbital entity registered twice");

  this->entityToOrbitalIdx[entity.id] = idx;
}
void EntityManager::registerObjectEntity(const Entity &entity, size_t idx)
{
  auto it = this->entityToObjectIdx.find(entity.id);
  if (it != this->entityToObjectIdx.end())
    Logger::logFatal("Entity Manager", "Object entity registered twice");

  this->entityToObjectIdx[entity.id] = idx;
}
void EntityManager::registerModelEntity(const Entity &entity, size_t idx)
{
  auto it = this->entityToModelIdx.find(entity.id);
  if (it != this->entityToModelIdx.end())
    Logger::logFatal("Entity Manager", "Model entity registered twice");

  this->entityToModelIdx[entity.id] = idx;
}

std::vector<std::unique_ptr<Entity>> &EntityManager::getEntities()
{
  return this->entities;
};

const std::vector<std::unique_ptr<Entity>> &EntityManager::getEntities() const
{
  return this->entities;
};
size_t EntityManager::getOrbitalIndex(const Entity &entity) const
{
  auto it = this->entityToOrbitalIdx.find(entity.id);
  if (it == this->entityToOrbitalIdx.end())
    Logger::logFatal("Entity Manager", "Orbital entity was not registered");

  return this->entityToOrbitalIdx.at(entity.id);
}
size_t EntityManager::getObjectIndex(const Entity &entity) const
{
  auto it = this->entityToObjectIdx.find(entity.id);
  if (it == this->entityToObjectIdx.end())
    Logger::logFatal("Entity Manager", "Object entity was not registered");

  return this->entityToObjectIdx.at(entity.id);
}
size_t EntityManager::getModelIndex(const Entity &entity) const
{
  auto it = this->entityToModelIdx.find(entity.id);
  if (it == this->entityToModelIdx.end())
    Logger::logFatal("Entity Manager", "Model entity was not registered");

  return this->entityToModelIdx.at(entity.id);
}
bool EntityManager::getIsOrbital(const Entity &entity) const
{
  auto it = this->entityToOrbitalIdx.find(entity.id);
  if (it == this->entityToOrbitalIdx.end())
    return false;

  return true;
}