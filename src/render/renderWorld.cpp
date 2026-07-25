#include "render/renderWorld.h"

#include "render/updatable.h"

// Public functions
void RenderWorld::update(const Camera &camera)
{
  for (Updatable *&object : this->updatable)
    object->update(camera);

  for (std::unique_ptr<Trail> &trail : this->trails)
    trail->update(camera);
}

void RenderWorld::addRenderable(Renderable *object)
{
  this->renderable.push_back(object);
}

void RenderWorld::addRenderSystem(RenderSystem *system)
{
  this->renderSystems.push_back(system);
}

void RenderWorld::addModelSource(ModelSource *object)
{
  this->modelSources.push_back(object);
}

void RenderWorld::addUpdatable(Updatable *object)
{
  this->updatable.push_back(object);
}

void RenderWorld::addTrail(std::unique_ptr<Trail> trail)
{
  this->trailViews.push_back(trail.get());
  this->trails.push_back(std::move(trail));
}

std::vector<Renderable *> &RenderWorld::getRenderable()
{
  if (this->renderable.empty())
    Logger::logWarning("RenderWorld", "Renderable is empty");

  return this->renderable;
};
std::vector<RenderSystem *> &RenderWorld::getRenderSystems()
{
  if (this->renderSystems.empty())
    Logger::logWarning("RenderWorld", "Render systems are empty");

  return this->renderSystems;
};
std::vector<ModelSource *> &RenderWorld::getModelSources()
{
  if (this->modelSources.empty())
    Logger::logWarning("RenderWorld", "Model sources are empty");

  return this->modelSources;
};

std::vector<Trail *> &RenderWorld::getTrails()
{
  if (this->trails.empty())
    Logger::logWarning("RenderWorld", "Trails are empty");

  return this->trailViews;
};