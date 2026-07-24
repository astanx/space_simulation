#pragma once

#include "graphics/instanceLayouts.h"

#include <vector>
#include <glm/glm.hpp>

class Model;
class InstanceManager;
class RenderQueue;
struct LODResult;

struct RenderGroup
{
  Model *model;
  std::vector<InstanceModelMatrixParts> fullLODInstances;
  std::vector<InstanceModelMatrixParts> fullShadowInstances;
  std::vector<InstanceModelMatrixParts> fullReflectorInstances;
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

public:
  RenderQueueBuilder(std::vector<Model *> models);
  ~RenderQueueBuilder() = default;

  void submit(Model *model, const LODResult &lod, const Transform &transform);
  void merge(RenderQueueBuilder &builder);
  void finish(InstanceManager &instances, RenderQueue &queue);
};