#include "render/renderer/backend/rendererBackendGPU.h"

#include "scene/scene.h"

// Constructor / Destructor
RendererBackendGPU::RendererBackendGPU() = default;
RendererBackendGPU::~RendererBackendGPU() = default;

// Public functions
void RendererBackendGPU::update(Scene &scene, RenderContext &ctx)
{
  this->updateCameraUBO(scene, ctx);

  this->lightManager.updateDirUBO(scene.getDirLight());
  this->shadowManager.updateDirUBO();

  // fix todo
  this->lightManager.updatePointUBO(scene.getPointLight());
  this->shadowManager.updatePointShadowLightPosition(scene.getSunPosition());
  this->shadowManager.updatePointUBO();
}