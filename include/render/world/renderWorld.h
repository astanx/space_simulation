#pragma once

#include "physics/trail.h"

#include <memory>

class Updatable;
class Renderable;
class RenderSystem;
class ModelSource;
class Camera;

class RenderWorld
{
private:
  std::vector<Updatable *> updatable;
  std::vector<Renderable *> renderable;
  std::vector<RenderSystem *> renderSystems;
  std::vector<ModelSource *> modelSources;

  std::vector<std::unique_ptr<Trail>> trails;
  std::vector<Trail *> trailViews;

public:
  RenderWorld() = default;
  ~RenderWorld() = default;

  void update(const Camera &camera);

  void addRenderable(Renderable *object);
  void addRenderSystem(RenderSystem *system);
  void addModelSource(ModelSource *object);
  void addUpdatable(Updatable *object);
  void addTrail(std::unique_ptr<Trail> trail);

  std::vector<Renderable *> &getRenderable();
  std::vector<RenderSystem *> &getRenderSystems();
  std::vector<ModelSource *> &getModelSources();
  std::vector<Trail *> &getTrails();
};