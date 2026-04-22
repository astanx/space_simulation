#include "render/modelSource.h"

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

  if (this->model)
    this->model->setPosition(this->renderPosition);
}

void ModelSource::render(Shader &shader, Frustum *frustum, bool force) const
{
  if (!force && frustum && !frustum->isVisibleSphere(this->renderPosition, this->radius))
    return;

  if (model)
    model->render(shader);
}

void ModelSource::renderInstanced(Shader &shader, Frustum *frustum, bool force) const
{
  if (!force && frustum && !frustum->isVisibleSphere(this->renderPosition, this->radius))
    return;

  if (model)
    model->renderInstanced();
}
