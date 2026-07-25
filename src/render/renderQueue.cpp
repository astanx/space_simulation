#include "render/renderQueue.h"

#include "debug/logger.h"

#include "render/lodManager.h"
#include "render/instanceManager.h"
#include "render/frustum.h"
#include "render/lodResult.h"
#include "render/modelSource.h"
#include "render/renderBatch.h"
#include "render/renderSystem.h"
#include "render/renderQueueBuilder.h"

#include "scene/scene.h"

// Private functions
void RenderQueue::buildModelSource(ModelSource *source, RenderQueueBuilder &builder, LODManager &lod, Frustum *frustum, InstanceManager &instance, FrameContext ctx, float fov)
{
  LODResult res = lod.partitionObject(source, frustum, ctx.height, fov);

  Transform transform;
  transform.position = source->getRenderPosition();
  transform.orientation = source->getRenderOrientation();

  source->forEachModel([&builder, &res, &transform](Model &model)
                       { builder.submit(&model, res, transform); });
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

  std::vector<Model *> models;
  size_t index = 0;
  for (ModelSource *source : scene.getModelSources())
    source->forEachModel([&models, &index](Model &model)
                         { 
                          model.setQueueIndex(index++);
                          models.push_back(&model); });

  RenderQueueBuilder builder(models);
  for (ModelSource *source : scene.getModelSources())
    this->buildModelSource(source, builder, lod, &frustum, instance, ctx, fov);

  for (RenderSystem *system : scene.getRenderSystems())
    system->buildRenderQueue(*this, lod, instance, camera, &frustum, ctx.height);

  builder.finish(instance, *this);

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

void RenderQueue::addShadowBatch(RenderBatch batch)
{
  if (!batch.model->hasFlag(ModelFlags::CastsShadow))
    Logger::logWarning("Render queue", "Model without shadow is added to shadow batch");
  this->shadowBatches.push_back(batch);
}

void RenderQueue::addReflectorBatch(RenderBatch batch)
{
  if (!batch.model->hasFlag(ModelFlags::ReflectsLight))
    Logger::logWarning("Render queue", "Model without reflection is added to reflector batch");
  this->reflectorBatches.push_back(batch);
}