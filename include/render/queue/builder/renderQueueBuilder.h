#pragma once

#include "graphics/instanceLayouts.h"

#include "resources/transform.h"

#include <vector>
#include <glm/glm.hpp>

class Model;
class InstanceManager;
class LODManager;
class RenderQueue;
class RenderSystem;
class ModelSource;
class Camera;
struct LODResult;
struct Frustum;
struct FrameContext;
struct RenderDatabaseView;
struct Entity;

struct RenderGroup
{
  Model *model;
  std::vector<InstanceModelMatrixParts> fullLODInstances;
  std::vector<InstanceModelMatrixParts> fullNonLODInstances;
};

class RenderQueueBuilder
{
private:
  std::vector<RenderGroup> groups;
  std::vector<InstancePositionRadiusTexture> impostors;
  std::vector<InstancePositionRadiusColor> points;

  void buildEntity(const Entity &entity, const RenderDatabaseView &database, LODManager &lod, Frustum *frustum, FrameContext ctx, float fov);

public:
  RenderQueueBuilder(std::vector<Model *> models);
  ~RenderQueueBuilder() = default;

  void build(RenderQueue &queue, const RenderDatabaseView &database, LODManager &lod, InstanceManager &instance, FrameContext &ctx);

  void submit(const Model *model, const LODResult &lod, const Transform &transform);
  void merge(RenderQueueBuilder &builder);
  void finish(InstanceManager &instances, RenderQueue &queue);
};