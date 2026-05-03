#include "render/modelSource.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedPolygonOffset.h"

#include "physics/constants.h"

// Private functions
void ModelSource::updateRenderPosition(glm::dvec3 realPosition)
{
  this->renderPosition = this->realToVisualPos(realPosition);
}

// Static functions
glm::dvec3 ModelSource::realToVisualPos(glm::dvec3 pos)
{
  return glm::dvec3(
             pos.x,
             -pos.z, // Z - Y
             pos.y   // Y - -Z
             ) *
         VISUAL_SCALE;
}

// Constructor
ModelSource::ModelSource(const PositionSource *src, double radius)
{
  this->src = src;
  this->radius = radius;
  this->updateRenderPosition(src->getPosition());
}

// Public functions
void ModelSource::update(double dt, Frustum *frustum, bool force)
{
  this->updateRenderPosition(src->getPosition());

  if (this->mainLayer)
    this->mainLayer->setPosition(this->renderPosition);

  for (auto &layer : this->layers)
    layer->setPosition(this->renderPosition);
}

void ModelSource::render(Shader &shader, Frustum *frustum, bool force) const
{
  if (!force && frustum && !frustum->isVisibleSphere(this->renderPosition, this->radius))
    return;

  {
    ScopedPolygonOffset offset(true, 5.f, 5.f);
    this->mainLayer->render(shader);
  }

  ScopedBlending blend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ScopedDepthMask mask(GL_FALSE);
  for (auto &layer : this->layers)
    layer->render(shader);
}

void ModelSource::renderInstanced(Shader &shader, Frustum *frustum, bool force) const
{
  if (!force && frustum && !frustum->isVisibleSphere(this->renderPosition, this->radius))
    return;

  for (auto &layer : this->layers)
    layer->renderInstanced();
}
