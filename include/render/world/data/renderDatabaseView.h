#pragma once

#include "scene/world/data/sharedDatabaseView.h"
#include "render/world/data/renderDatabase.h"

#include "resources/entity/entityManager.h"

class PointLight;
class Camera;

struct RenderDatabaseView
{
private:
  const EntityManager &entityManager;
  const Camera &camera;

  const SharedDatabaseView &shared;
  const RenderDatabase &render;

public:
  RenderDatabaseView(const EntityManager &entityManager, const Camera &camera, const SharedDatabaseView &shared, const RenderDatabase &render) : entityManager(entityManager), camera(camera), shared(shared), render(render) {};
  ~RenderDatabaseView() = default;

  // Entity
  const std::vector<Entity> &getEntities() const { return this->entityManager.getEntities(); };
  template <typename F>
  void forEachSpecialEntity(F &&func) const { this->entityManager.forEachSpecialEntity(func); };

  // Render
  const Camera &getCamera() const { return this->camera; };
  size_t getModelsCount() const { return this->render.models.size(); };
  size_t getEntitiesCount() const { return this->entityManager.getEntities().size(); };
  Model *getModel(const Entity &entity) const { return this->render.getModel(this->entityManager.getModelIndex(entity)); };
  const std::vector<Model *> &getModels() const { return this->render.models; };

  // Shared
  Transform getTransform(const Entity &entity) const { return this->shared.getTransform(this->entityManager.getObjectIndex(entity), this->camera); };
  Radii getRadii(const Entity &entity) const { return this->shared.getRadii(this->entityManager.getObjectIndex(entity)); };
  void moveLight(const Entity &entity, PointLight &light) const { this->shared.moveLight(this->entityManager.getObjectIndex(entity), this->camera, light); };
};