#include "render/world/backend/renderWorldBackendCPU.h"

#include "render/world/data/renderDatabaseView.h"

#include "render/queue/builder/renderQueueBuilder.h"

#include "render/trail/trailManager.h"

#include "render/modelSource.h"
#include "render/renderSystem.h"
#include "render/renderContext.h"

#include "physics/world/physicsWorld.h"

// Constructor
RenderWorldBackendCPU::RenderWorldBackendCPU() : lodManager(this->lodSettings) {};

// Public functions
void RenderWorldBackendCPU::update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, TrailManager &trailManager, RenderContext &ctx)
{
  this->initSubQueues(queue, instanceManager, database);

  RenderQueueBuilder builder(database.getModels());
  builder.build(queue, database, this->lodManager, instanceManager, ctx.frameCtx);

  database.forEachSpecialEntity([this, &database, &trailManager, &ctx](const Entity entity)
                                {
                                  if (!ctx.settings.paused)
                                    trailManager.addTrailPosition(entity, database.getPosition(entity));
                                  trailManager.updateTrail(entity, database.getCamera()); });
}

void RenderWorldBackendCPU::sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light)
{
  if (light)
    database.moveLight(physics.getSun(), *light);
  else
    Logger::logFatal("Scene", " No sun light to sync position");

  database.forEachSpecialEntity([this, &database](const Entity entity)
                                {
      glm::vec3 pos = database.getTransform(entity).position;

      this->updateSpecialModel(database.getModel(entity), pos); });
}