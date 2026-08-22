#include "resources/entity/entityManager.h"

#include "debug/logger.h"

// Public functions
const Entity &EntityManager::create()
{
  Entity entity{this->entities.size()};
  this->entities.push_back(entity);
  return entity;
};

void EntityManager::registerOrbitalEntity(const Entity &entity, size_t idx)
{
  auto it = this->entityToOrbitalIdx.find(&entity);
  if (it != this->entityToOrbitalIdx.end())
    Logger::logFatal("Entity Manager", "Orbital entity registered twice");

  this->entityToOrbitalIdx[&entity] = idx;
}
void EntityManager::registerObjectEntity(const Entity &entity, size_t idx)
{
  auto it = this->entityToObjectIdx.find(&entity);
  if (it != this->entityToObjectIdx.end())
    Logger::logFatal("Entity Manager", "Object entity registered twice");

  this->entityToObjectIdx[&entity] = idx;
}
void EntityManager::registerModelEntity(const Entity &entity, size_t idx)
{
  auto it = this->entityToModelIdx.find(&entity);
  if (it != this->entityToModelIdx.end())
    Logger::logFatal("Entity Manager", "Model entity registered twice");

  this->entityToModelIdx[&entity] = idx;
}

const std::vector<Entity> &EntityManager::getEntities() const
{
  return this->entities;
};
size_t EntityManager::getOrbitalIndex(const Entity &entity) const
{
  auto it = this->entityToOrbitalIdx.find(&entity);
  if (it == this->entityToOrbitalIdx.end())
    Logger::logFatal("Entity Manager", "Orbital entity was not registered");

  return this->entityToOrbitalIdx.at(&entity);
}
size_t EntityManager::getObjectIndex(const Entity &entity) const
{
  auto it = this->entityToObjectIdx.find(&entity);
  if (it == this->entityToObjectIdx.end())
    Logger::logFatal("Entity Manager", "Object entity was not registered");

  return this->entityToObjectIdx.at(&entity);
}
size_t EntityManager::getModelIndex(const Entity &entity) const
{
  auto it = this->entityToModelIdx.find(&entity);
  if (it == this->entityToModelIdx.end())
    Logger::logFatal("Entity Manager", "Model entity was not registered");

  return this->entityToModelIdx.at(&entity);
}