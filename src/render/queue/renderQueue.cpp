#include "render/queue/renderQueue.h"

#include "debug/logger.h"

#include "render/renderBatch.h"

#include "graphics/model.h"

// Public functions
void RenderQueue::clear()
{
  this->coreBatches.clear();
  this->tangentBatches.clear();
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