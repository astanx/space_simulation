#include "render/world/backend/renderWorldBackend.h"

#include "render/instanceManager.h"
#include "render/queue/renderQueue.h"

#include "graphics/model.h"

// Protected function
void RenderWorldBackend::initSubQueues(RenderQueue &queue, InstanceManager &manager, std::vector<Model *> &models)
{
  if (this->subQueuesInitialized)
    return;

  this->subQueuesInitialized = true;

  for (Model *model : models)
  {
    Range allocation = manager.getAllocation(model);
    if (model->hasFlag(ModelFlags::CastsShadow))
      queue.addShadowBatch({model, allocation});

    if (model->hasFlag(ModelFlags::ReflectsLight))
      queue.addReflectorBatch({model, allocation});
  }
}