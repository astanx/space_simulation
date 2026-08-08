#include "render/lod/manager/lodManager.h"

#include "render/lod/lodFunctions.h"

#include "render/lod/lodResult.h"
#include "render/modelSource.h"
#include "render/frustum.h"
#include "render/renderFlags.h"
#include "render/instanceManager.h"
#include "render/renderSystem.h"

#include "debug/logger.h"

#include "physics/systems/system.h"

#include "maths/constants.h"

#include "camera/camera.h"

#include "scene/scene.h"

#include "graphics/model.h"

#include "graphics/primitives/quad.h"
#include "graphics/primitives/point.h"

#include "graphics/bindings/impostor.h"

#include "graphics/state/scopedTexture.h"

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "external/stb_image_resize2.h"

// Private functions
void LODManager::initImpostor(std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems)
{
  this->impostorMesh = std::make_unique<Mesh>(TypeTag<VertexPositionTexcoord>{}, std::make_unique<Quad>(), VertexLayout::PositionTexcoord);
  this->impostorMesh->setInstanceLayout(InstanceLayout::PositionRadiusTexture);

  this->impostorTexture = std::make_unique<Texture>(GL_TEXTURE_2D_ARRAY);

  unsigned int layer = ImpostorTextureBindingPoints::Size;
  for (ModelSource *source : modelSources)
    source->forEachModel([this, &layer](Model &model)
                         { this->bindLayerToImpostorTexture(model, layer++); });

  for (RenderSystem *system : renderSystems)
    for (Model *model : system->getModels())
      this->bindLayerToImpostorTexture(model, layer++);
}

void LODManager::initPoint()
{
  this->pointMesh = std::make_unique<Mesh>(TypeTag<VertexEmpty>{}, std::make_unique<Point>(), VertexLayout::Empty, GL_POINTS);
  this->pointMesh->setInstanceLayout(InstanceLayout::PositionRadiusColor);
}

void LODManager::bindLayerToImpostorTexture(Model &model, unsigned int layer)
{
  if (!this->impostorTexture)
    Logger::logFatal("LOD Manager", "No importor texture to bind layer");

  ScopedTexture impostor(*this->impostorTexture);

  int width = 1024;
  int height = 512;
  const Material *mat = model.getMaterial();

  if (!mat)
  {
    Logger::logWarning("LOD Manager", "No material to get texture");
    return;
  }

  const Texture *texture = mat->getTexture();

  if (!texture)
  {
    Logger::logWarning("LOD Manager", "No texture to bind impostor layer");
    return;
  }

  int srcWidth = texture->getWidth();
  int srcHeight = texture->getHeight();

  if (!this->isImpostorInitialized)
  {
    GL_CALL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, ImpostorTextureBindingPoints::MaxSize));
    this->isImpostorInitialized = true;
  }

  if (layer >= ImpostorTextureBindingPoints::MaxSize)
    Logger::logError("LOD Manager", "Maximum impostor layer size exceeded");

  std::vector<uint8_t> pixels(srcWidth * srcHeight * 4);

  {
    ScopedTexture text(*texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  }

  std::vector<uint8_t> resized(width * height * 4);

  stbir_resize_uint8_linear(pixels.data(), srcWidth, srcHeight, 0, resized.data(), width, height, 0, STBIR_RGBA);

  GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, resized.data()));

  model.setImpostorLayer(layer);
}

void LODManager::bindLayerToImpostorTexture(Model *model, unsigned int layer)
{
  if (!model)
  {
    Logger::logWarning("LOD Manager", "Model is not passed for impostor binding");
    return;
  }

  this->bindLayerToImpostorTexture(*model, layer);
}

uint LODManager::getLODLevel(float pixelRadius)
{
  return ::getLODLevelFromPixelRadius(pixelRadius, this->settings.fullThreshold, this->settings.impostorThreshold);
}

uint LODManager::getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  return ::getLODLevel(position, radius, fov, viewportHeight, importance, this->settings.baseMinPixelSize, this->settings.fullThreshold, this->settings.impostorThreshold);
}

// Public functions
void LODManager::init(std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems)
{
  this->initImpostor(modelSources, renderSystems);
  this->initPoint();
}

float LODManager::calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  return ::calculatePixelRadius(position, radius, fov, viewportHeight, importance, this->settings.baseMinPixelSize);
}

float LODManager::scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  return ::scaleRadius(position, radius, fov, viewportHeight, importance, this->settings.baseMinPixelSize);
}

LODResult LODManager::partitionObject(const glm::vec3 &position, float importance, Radii radii, Frustum *frustum, float viewportHeight, float fov)
{
  LODResult result;
  float radius = radii.mean;
  result.scaledMeanRadius = this->scaleRadius(position, radius, fov, viewportHeight, importance);

  if (!shouldBeProcessed(frustum, position, result.scaledMeanRadius))
  {
    result.visible = false;
    return result;
  }

  result.visible = true;

  result.level = this->getLODLevel(position, radius, fov, viewportHeight, importance);

  if (result.level == LOD_FULL)
  {
    result.scaledEquatorianRadius = this->scaleRadius(position, radii.equatorian, fov, viewportHeight, importance);
    result.scaledPolarRadius = this->scaleRadius(position, radii.polar, fov, viewportHeight, importance);

    result.equatorianScale = result.scaledEquatorianRadius / radii.equatorian;
    result.polarScale = result.scaledPolarRadius / radii.polar;
  }

  return result;
}