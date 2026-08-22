#include "render/world/backend/renderWorldBackend.h"

#include "render/instanceManager.h"
#include "render/reflectanceAcceptor.h"
#include "render/queue/renderQueue.h"

#include "render/world/data/renderDatabaseView.h"

#include "graphics/model.h"

#include "scene/light/pointLight.h"

#include "physics/world/physicsWorld.h"

// Protected function
void RenderWorldBackend::initShadowQueue(RenderQueue &queue, InstanceManager &manager, const Model *model)
{
  if (model->hasFlag(ModelFlags::CastsShadow))
  {
    Range allocation = manager.getAllocation(model);
    queue.addShadowBatch({model, allocation});
  }
}
void RenderWorldBackend::initReflectorQueue(RenderQueue &queue, InstanceManager &manager, const Model *model)
{
  const ReflectanceAcceptor *acceptor = dynamic_cast<const ReflectanceAcceptor *>(model);
  if (acceptor)
  {
    Range allocation = manager.getAllocation(model);
    const Model *reflector = acceptor->getReflector();
    if (!reflector->hasFlag(ModelFlags::ReflectsLight))
      Logger::logFatal("Render World Backend", "Reflector does not reflect light");

    queue.addReflectorBatch({acceptor, allocation, {reflector, manager.getAllocation(reflector)}});
  }
}
void RenderWorldBackend::initModelQueue(RenderQueue &queue, InstanceManager &manager, const Model *model)
{
  this->initShadowQueue(queue, manager, model);
  this->initReflectorQueue(queue, manager, model);
}

void RenderWorldBackend::initSubQueues(RenderQueue &queue, InstanceManager &manager, const std::vector<Model *> &models)
{
  if (this->lastModelsSize == models.size())
    return;

  queue.clearSubQueues();

  this->lastModelsSize = models.size();

  for (Model *model : models)
    this->initModelQueue(queue, manager, model);
}