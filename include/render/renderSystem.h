#pragma once

class RenderQueue;
class LODManager;
class InstanceManager;
class Camera;
class Texture;
struct Frustum;

class RenderSystem
{
protected:
  std::vector<Model *> models;
  float importance;

public:
  virtual void buildRenderQueue(RenderQueue &queue, LODManager &lod, InstanceManager &instances, const Camera &camera, Frustum *frustum, float viewportHeight) = 0;

  std::vector<Model *> &getModels() { return this->models; };
};