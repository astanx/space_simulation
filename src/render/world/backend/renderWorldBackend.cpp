#include "render/world/backend/renderWorldBackend.h"

#include "render/instanceManager.h"
#include "render/reflector.h"
#include "render/reflectanceAcceptor.h"
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

    Reflector *reflector = dynamic_cast<Reflector *>(model);
    if (reflector && reflector->hasFlag(ModelFlags::ReflectsLight))
    {
      ReflectanceAcceptor *acceptor = reflector->getAcceptor();
      queue.addReflectorBatch({acceptor, manager.getAllocation(acceptor), {reflector, allocation}});
    }
  }
}