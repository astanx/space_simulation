#include "render/world/backend/renderWorldBackend.h"

#include "render/instanceManager.h"
#include "render/reflectanceAcceptor.h"
#include "render/queue/renderQueue.h"

#include "graphics/model.h"

#include "scene/light/pointLight.h"

#include "physics/world/physicsWorld.h"
#include "physics/star.h"

// Protected function
void RenderWorldBackend::initShadowQueue(RenderQueue &queue, InstanceManager &manager, Model *model)
{
  if (model->hasFlag(ModelFlags::CastsShadow))
  {
    Range allocation = manager.getAllocation(model);
    queue.addShadowBatch({model, allocation});
  }
}
void RenderWorldBackend::initReflectorQueue(RenderQueue &queue, InstanceManager &manager, Model *model)
{
  ReflectanceAcceptor *acceptor = dynamic_cast<ReflectanceAcceptor *>(model);
  if (acceptor)
  {
    Range allocation = manager.getAllocation(model);
    Model *reflector = acceptor->getReflector();
    if (!reflector->hasFlag(ModelFlags::ReflectsLight))
      Logger::logFatal("Render World Backend", "Reflector does not reflect light");

    queue.addReflectorBatch({acceptor, allocation, {reflector, manager.getAllocation(reflector)}});
  }
}
void RenderWorldBackend::initModelQueue(RenderQueue &queue, InstanceManager &manager, Model *model)
{
  this->initShadowQueue(queue, manager, model);
  this->initReflectorQueue(queue, manager, model);
}

void RenderWorldBackend::initSubQueues(RenderQueue &queue, InstanceManager &manager, std::vector<Model *> &models)
{
  if (this->subQueuesInitialized)
    return;

  this->subQueuesInitialized = true;

  for (Model *model : models)
    this->initModelQueue(queue, manager, model);
}

void RenderWorldBackend::moveSunLight(glm::vec3 position, PointLight *light)
{
  if (light)
    light->move(position); // move sun light
  else
    Logger::logFatal("Scene", " No sun light to sync position");
}
