#include "render/modelSource.h"

#include "camera/camera.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedPolygonOffset.h"

#include "physics/constants.h"

// Constructor
ModelSource::ModelSource(const TransformSource &src, double renderRadius) : src(src)
{
  this->renderRadius = renderRadius;
}

// Public functions
void ModelSource::update(const Camera &camera)
{
  this->renderPosition = camera.worldToViewSpace(this->src.getPosition());
  this->renderOrientation = camera.worldToViewSpace(this->src.getOrientation());

  if (this->mainLayer)
  {
    this->mainLayer->setPosition(this->renderPosition);
    this->mainLayer->setOrientation(this->renderOrientation);
  }

  for (auto &layer : this->layers)
    layer->setPosition(this->renderPosition);
}

void ModelSource::render(Shader &shader)
{
  ScopedPolygonOffset offset(true, .1f, 4.f);
  this->mainLayer->render(shader);
}

void ModelSource::renderLayers(Shader &shader) const
{
  ScopedBlending blend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ScopedDepthMask mask(GL_FALSE);
  for (auto &layer : this->layers)
    layer->render(shader);
}

void ModelSource::renderInstanced()
{
  for (auto &layer : this->layers)
    layer->renderInstanced();
}

void ModelSource::scaleRadii(Radii scaledRadii)
{
  this->renderRadius = scaledRadii.mean;
  Radii radii = this->src.getRadii();

  float equatorian = scaledRadii.equatorian / radii.equatorian;
  float polar = scaledRadii.polar / radii.polar;

  glm::vec3 scale(equatorian, polar, equatorian);

  this->mainLayer->setScale(scale);

  for (auto &layer : this->layers)
    layer->setScale(scale);
}

void ModelSource::forEachModel(std::function<void(Model &, RenderFlags)> &&func)
{
  func(*this->mainLayer, RenderFlags::Main);

  for (auto &layer : this->layers)
    func(*layer, RenderFlags::Layer);
}
