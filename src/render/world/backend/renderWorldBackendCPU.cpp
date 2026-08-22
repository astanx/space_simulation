#include "render/world/backend/renderWorldBackendCPU.h"

#include "render/world/data/renderDatabaseView.h"

#include "render/queue/builder/renderQueueBuilder.h"

#include "render/modelSource.h"
#include "render/renderSystem.h"

#include "physics/world/physicsWorld.h"

// Constructor
RenderWorldBackendCPU::RenderWorldBackendCPU() : lodManager(this->lodSettings) {};

// Public functions
void RenderWorldBackendCPU::update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, FrameContext &ctx)
{
  const std::vector<Model *> &models = database.getModels();
  if (this->lastModelsSize != database.getModelsCount())
    this->initSubQueues(queue, instanceManager, models);

  RenderQueueBuilder builder(models);
  builder.build(queue, database, this->lodManager, instanceManager, ctx);
}

void RenderWorldBackendCPU::sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light)
{
  if (light)
    database.moveLight(physics.getSun(), *light);
  else
    Logger::logFatal("Scene", " No sun light to sync position");
}