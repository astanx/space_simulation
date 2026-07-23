#include "render/renderQueue.h"

#include "debug/logger.h"

#include "render/lodManager.h"
#include "render/instanceManager.h"
#include "render/frustum.h"
#include "render/lodResult.h"
#include "render/modelSource.h"
#include "render/renderBatch.h"
#include "render/renderSystem.h"

#include "scene/scene.h"

// Private functions
void RenderQueue::buildModelSource(ModelSource *source, LODManager &lod, Frustum *frustum, InstanceManager &instance, FrameContext ctx, float fov)
{
  LODResult res = lod.partitionObject(source, frustum, ctx.height, fov);

  switch (res.level)
  {
  case LOD::Full:
  {
    InstanceModelMatrixParts data;
    data.position = source->getRenderPosition();
    data.orientation = source->getRenderOrientation();
    data.scale = glm::vec3(res.equatorianScale, res.polarScale, res.equatorianScale);
    Range range = instance.add(data);

    source->forEachModel([this, &range](Model &model, RenderFlags flag)
                         { 
                            if (model.getIsTangent())
                              this->addTangentBatch({&model, range, flag});
                            else 
                              this->addCoreBatch({&model, range, flag}); });
    break;
  }

  case LOD::Impostor:
  {
    source->forEachModel([this, &instance, source, res](Model &model)
                         { 
                      InstancePositionRadiusTexture data;
                      data.position = source->getRenderPosition();
                      data.radius = res.scaledMeanRadius;
                      data.textureLayer = model.getImpostorLayer();
                      Range range = instance.add(data); });
    break;
  }

  case LOD::Point:
  {
    InstancePositionRadiusColor data;
    data.position = source->getRenderPosition();
    data.radius = res.scaledMeanRadius;
    data.color = source->getMainLayerTexture()->getAverageColor();
    Range range = instance.add(data);
    break;
  }

  default:
    Logger::logError("Render Queue", "No handler for LOD level: " + std::to_string(res.level));
    break;
  }
}

void RenderQueue::clear()
{
  this->coreBatches.clear();
  this->tangentBatches.clear();
}

// Public functions
void RenderQueue::build(Scene &scene, LODManager &lod, InstanceManager &instance, FrameContext &ctx)
{
  this->clear();
  instance.clear();

  const Camera &camera = scene.getActiveCamera();
  float fov = camera.getFOV();
  Frustum frustum = scene.getActiveCamera().getFrustum(ctx.aspect);

  for (ModelSource *source : scene.getModelSources())
    this->buildModelSource(source, lod, &frustum, instance, ctx, fov);

  for (RenderSystem *system : scene.getRenderSystems())
    system->buildRenderQueue(*this, lod, instance, camera, &frustum, ctx.height);

  instance.fillVBOs();
}

void RenderQueue::addCoreBatch(RenderBatch batch)
{
  if (batch.model->getIsTangent())
    Logger::logWarning("Render queue", "Model with tangent is added to core batch");
  this->coreBatches.push_back(batch);
}
void RenderQueue::addTangentBatch(RenderBatch batch)
{
  if (!batch.model->getIsTangent())
    Logger::logWarning("Render queue", "Model without tangent is added to tangent batch");
  this->tangentBatches.push_back(batch);
}