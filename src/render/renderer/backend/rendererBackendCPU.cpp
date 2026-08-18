#include "render/renderer/backend/rendererBackendCPU.h"

#include "scene/scene.h"
#include "scene/light/pointLight.h"

// Constructor / Destructor
RendererBackendCPU::RendererBackendCPU() = default;
RendererBackendCPU::~RendererBackendCPU() = default;

// Public functions
void RendererBackendCPU::update(Scene &scene, RenderContext &ctx)
{
  this->updateCameraUBO(scene, ctx);

  this->lightManager.updateDirUBO(scene.getDirLight());
  this->shadowManager.updateDirUBO();

  this->lightManager.updatePointUBO(scene.getPointLight());
  this->shadowManager.updatePointShadowLightPosition(scene.getPointLight()->getPosition());
  this->shadowManager.updatePointUBO();
}