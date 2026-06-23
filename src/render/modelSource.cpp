#include "render/modelSource.h"

#include "camera/camera.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedPolygonOffset.h"

#include "physics/constants.h"

// Private functions
void ModelSource::renderLayers(Shader &shader) const
{
  ScopedBlending blend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ScopedDepthMask mask(GL_FALSE);
  for (auto &layer : this->layers)
    layer->render(shader);
}

// Constructor
ModelSource::ModelSource(const PositionSource &src, double radius) : src(src)
{
  this->radius = radius;
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

void ModelSource::render(Shader &shader, Frustum *frustum, bool force) const
{
  if (!force && frustum && !frustum->isVisibleSphere(this->renderPosition, this->radius))
    return;

  {
    ScopedPolygonOffset offset(true, .1f, 4.f);
    this->mainLayer->render(shader);
  }

  this->renderLayers(shader);
}

void ModelSource::renderInstanced(Shader &shader, Frustum *frustum, bool force) const
{
  if (!force && frustum && !frustum->isVisibleSphere(this->renderPosition, this->radius))
    return;

  for (auto &layer : this->layers)
    layer->renderInstanced();
}
