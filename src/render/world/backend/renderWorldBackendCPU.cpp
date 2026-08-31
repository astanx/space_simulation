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
  this->initSubQueues(queue, instanceManager, database);

  RenderQueueBuilder builder(database.getModels());
  builder.build(queue, database, this->lodManager, instanceManager, ctx);
}

void RenderWorldBackendCPU::sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light)
{
  if (light)
    database.moveLight(physics.getSun(), *light);
  else
    Logger::logFatal("Scene", " No sun light to sync position");

  // todo update reflector pos
}