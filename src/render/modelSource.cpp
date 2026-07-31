#include "render/modelSource.h"

#include "render/instanceManager.h"

#include "camera/camera.h"

#include "graphics/shader.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedPolygonOffset.h"

#include "physics/constants.h"

#include <glm/gtc/matrix_transform.hpp>

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

  this->modelMatrix = glm::mat4(1.f);
  this->modelMatrix = glm::translate(this->modelMatrix, this->renderPosition);
  this->modelMatrix *= glm::mat4(this->renderOrientation);
  this->modelMatrix = glm::scale(this->modelMatrix, this->renderScale);
}

void ModelSource::render(Shader &shader)
{
  shader.setMat4fv(this->modelMatrix, "ModelMatrix");
  ScopedPolygonOffset offset(true, .1f, 4.f);
  this->mainLayer->render(shader);
}

void ModelSource::renderLayers(Shader &shader) const
{
  shader.setMat4fv(this->modelMatrix, "ModelMatrix");
  ScopedBlending blend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ScopedDepthMask mask(GL_FALSE);
  for (auto &layer : this->layers)
    layer->render(shader);
}

void ModelSource::renderLayersInstanced(Shader &shader) const
{
  ScopedBlending blend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ScopedDepthMask mask(GL_FALSE);
  for (auto &layer : this->layers)
    layer->renderInstanced(shader);
}

void ModelSource::renderInstanced(Shader &shader, Buffer *instanceVBO, size_t size, size_t count, size_t offset)
{
  ScopedPolygonOffset polygonOffset(true, .1f, 4.f);
  this->mainLayer->renderInstanced(shader, instanceVBO, size, count, offset);
}

void ModelSource::scaleRadii(Radii scaledRadii)
{
  this->renderRadius = scaledRadii.mean;
  Radii radii = this->src.getRadii();

  float equatorian = scaledRadii.equatorian / radii.equatorian;
  float polar = scaledRadii.polar / radii.polar;

  this->renderScale = glm::vec3(equatorian, polar, equatorian);
}

void ModelSource::reserveInstances(InstanceManager &instanceManager)
{
  this->forEachModel([&instanceManager](Model &model)
                     { instanceManager.reserve(&model, 1); });
}
