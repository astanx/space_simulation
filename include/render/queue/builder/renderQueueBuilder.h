#pragma once

#include "graphics/instanceLayouts.h"

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

struct RenderGroup
{
  Model *model;
  std::vector<InstanceModelMatrixParts> fullLODInstances;
  std::vector<InstanceModelMatrixParts> fullNonLODInstances;
};

struct Transform
{
  glm::vec3 position;
  glm::quat orientation;
};

class RenderQueueBuilder
{
private:
  std::vector<RenderGroup> groups;
  std::vector<InstancePositionRadiusTexture> impostors;
  std::vector<InstancePositionRadiusColor> points;

  void buildModelSource(ModelSource *source, LODManager &lod, Frustum *frustum, FrameContext ctx, float fov);

public:
  RenderQueueBuilder(std::vector<Model *> models);
  ~RenderQueueBuilder() = default;

  void build(RenderQueue &queue, const Camera& camera, std::vector<ModelSource*> &modelSources, std::vector<RenderSystem*> renderSystems, LODManager &lod, InstanceManager &instance, FrameContext &ctx);

  void submit(Model *model, const LODResult &lod, const Transform &transform);
  void merge(RenderQueueBuilder &builder);
  void finish(InstanceManager &instances, RenderQueue &queue);
};