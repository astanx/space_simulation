#include "render/lodManager.h"

#include "render/modelSource.h"
#include "render/frustum.h"
#include "render/renderFlags.h"

#include "debug/logger.h"

#include "physics/systems/system.h"

#include "maths/constants.h"

#include "camera/camera.h"

#include "scene/scene.h"

#include "graphics/primitives/quad.h"
#include "graphics/primitives/point.h"

#include "graphics/bindings/impostor.h"

#include "graphics/state/scopedTexture.h"

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "external/stb_image_resize2.h"

// Private functions
void LODManager::initImpostor(Scene &scene)
{
  this->impostorMesh = std::make_unique<Mesh>(TypeTag<VertexPositionTexcoord>{}, std::make_unique<Quad>(), VertexLayout::PositionTexcoord);
  this->impostorMesh->setInstanceLayout(InstanceLayout::PositionRadiusTexture);
  this->impostorMesh->setInstanceBuffer(this->impostorInstances.data(), this->impostorInstances.size());

  this->impostorTexture = std::make_unique<Texture>(GL_TEXTURE_2D_ARRAY);

  unsigned int layer = ImpostorTextureBindingPoints::Size;
  for (ModelSource *object : scene.getModelSources())
  {
    const Texture *texture = object->getMainLayerTexture();
    if (!texture)
      Logger::logWarning("LOD Manager", "No texture found for object");

    this->bindLayerToImpostorTexture(*texture, layer);

    object->setImpostorLayer(layer);
    layer++;
  }

  for (System *system : scene.getPhysicsWorld().getSystems())
  {
    const Texture *texture = system->getTexture();
    if (!texture)
      Logger::logWarning("LOD Manager", "No texture found for system");

    this->bindLayerToImpostorTexture(*texture, layer);

    system->setImpostorLayer(layer);
    layer++;
  }
}

void LODManager::initPoint()
{
  this->pointMesh = std::make_unique<Mesh>(TypeTag<VertexEmpty>{}, std::make_unique<Point>(), VertexLayout::Empty, GL_POINTS);
  this->pointMesh->setInstanceLayout(InstanceLayout::PositionRadiusColor);
  this->pointMesh->setInstanceBuffer(this->pointInstances.data(), this->pointInstances.size());
}

void LODManager::bindLayerToImpostorTexture(const Texture &texture, unsigned int layer)
{
  if (!this->impostorTexture)
    Logger::logFatal("LOD Manager", "No importor texture to bind layer");

  ScopedTexture impostor(*this->impostorTexture);

  int width = 1024;
  int height = 512;

  int srcWidth = texture.getWidth();
  int srcHeight = texture.getHeight();

  if (!this->isImpostorInitialized)
  {
    GL_CALL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, ImpostorTextureBindingPoints::MaxSize));
    this->isImpostorInitialized = true;
  }

  if (layer >= ImpostorTextureBindingPoints::MaxSize)
    Logger::logError("LOD Manager", "Maximum impostor layer size exceeded");

  std::vector<uint8_t> pixels(srcWidth * srcHeight * 4);

  {
    ScopedTexture text(texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  }

  std::vector<uint8_t> resized(width * height * 4);

  stbir_resize_uint8_linear(pixels.data(), srcWidth, srcHeight, 0, resized.data(), width, height, 0, STBIR_RGBA);

  GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, resized.data()));
}

void LODManager::partitionObject(ModelSource *object, Frustum *frustum, float viewportHeight, float fov, bool force)
{
  float importance = object->getRenderImportance();
  float radius = object->getWorldRadius();
  const glm::dvec3 pos = object->getRenderPosition();
  float scaledMeanRadius = this->scaleRadius(pos, radius, fov, viewportHeight, importance);
  object->setRenderRadius(scaledMeanRadius);

  if (!Frustum::shouldBeProcessed(frustum, pos, scaledMeanRadius, force))
    return;

  const Texture *texture = object->getMainLayerTexture();

  int level = this->getLODLevel(pos, radius, fov, viewportHeight, importance);

  switch (level)
  {
  case LOD::Full:
  {
    const Radii &radii = object->getSrcRadii();
    float scaledEquatorianRadius = this->scaleRadius(pos, radii.equatorian, fov, viewportHeight, importance);
    float scaledPolarRadius = this->scaleRadius(pos, radii.polar, fov, viewportHeight, importance);
    object->scaleRadii(Radii{scaledEquatorianRadius, scaledPolarRadius, scaledMeanRadius});

    object->forEachModel([this](Model &model, int flags)
                         {
                          if ((flags & RenderFlags::Tangent) == RenderFlags::Tangent)
                            this->fullTangentInstances.insert({&model, flags});
                          else
                            this->fullInstances.insert({&model, flags}); });
    break;
  }
  case LOD::Impostor:
    this->impostorInstances.emplace_back(InstancePositionRadiusTexture{pos, scaledMeanRadius, object->getImpostorLayer()});
    break;
  case LOD::Point:
    this->pointInstances.emplace_back(InstancePositionRadiusColor{pos, scaledMeanRadius, texture->getAverageColor()});
    break;
  default:
    Logger::logError("LOD Manager", "No handler for LOD level: " + std::to_string(level));
    break;
  }
}

void LODManager::partitionObjects(Scene &scene, RenderContext &ctx)
{
  this->fullInstances.clear();
  this->impostorInstances.clear();
  this->pointInstances.clear();

  const Camera &camera = scene.getActiveCamera();
  Frustum frustum = scene.getActiveCamera().getFrustum(ctx.frameCtx.aspect);

  for (ModelSource *object : scene.getModelSources())
    this->partitionObject(object, &frustum, ctx.frameCtx.height, camera.getFOV());

  for (auto &system : scene.getPhysicsWorld().getSystems())
    system->partitionObjects(this->impostorInstances, this->pointInstances, camera, this, ctx.frameCtx.height, &frustum);
}

// Public functions
void LODManager::init(Scene& scene)
{
  this->initImpostor(scene);
  this->initPoint();
}
int LODManager::getLODLevel(float pixelRadius)
{
  if (pixelRadius >= this->settings.pixelRadiusThreshold[LOD::Full])
    return LOD::Full;
  else if (pixelRadius >= this->settings.pixelRadiusThreshold[LOD::Impostor])
    return LOD::Impostor;
  else
    return LOD::Point;
}

int LODManager::getLODLevel(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  float pixelRadius = this->calculatePixelRadius(position, radius, fov, viewportHeight, importance);

  return this->getLODLevel(pixelRadius);
}

float LODManager::calculatePixelRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  float d = glm::length(position);

  if (d < EPS)
    d = 1e-4f;

  float worldToPixel = (viewportHeight * 0.5f) / (d * tan(glm::radians(fov / 2.f)));

  float pixelRadius = radius * worldToPixel * importance;

  float minPixel = this->settings.baseMinPixelSize * importance;
  pixelRadius = std::max(pixelRadius, minPixel);

  return pixelRadius;
}

float LODManager::scaleRadius(const glm::vec3 &position, float radius, float fov, float viewportHeight, float importance)
{
  float minPixelSize = this->settings.baseMinPixelSize * importance;

  float pixelWorldSize = (length(position) * 2.f * tan(glm::radians(fov / 2.f))) / viewportHeight;
  float minWorldRadius = minPixelSize * pixelWorldSize * 0.5f;

  float finalRadius = std::max(radius, minWorldRadius);

  return finalRadius;
}

void LODManager::update(Scene& scene, RenderContext& ctx)
{
  this->partitionObjects(scene, ctx);
  this->impostorMesh->updateInstanceBuffer(this->impostorInstances.data(), this->impostorInstances.size());
  this->pointMesh->updateInstanceBuffer(this->pointInstances.data(), this->pointInstances.size());
}