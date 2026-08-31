#include "render/world/backend/renderWorldBackendGPU.h"

#include "debug/logger.h"

#include "render/world/backend/backendGPUBuffers.h"
#include "render/world/data/renderDatabaseView.h"
#include "render/queue/data/renderQueueGPUBuffers.h"

#include "compute/context.h"

#include "physics/world/physicsWorld.h"
#include "physics/world/total.h"

#include "scene/light/pointLight.h"

#include <iostream>

// Private functions
void RenderWorldBackendGPU::initSpecialModel(RenderQueue &queue, InstanceManager &manager, const RenderDatabaseView &database, const Entity entity)
{
  const Model *model = database.getModel(entity);
  if (!model->hasFlag(ModelFlags::Special))
    return;

  Range allocation = manager.getAllocation(model);
  if (allocation.end - allocation.begin != 1)
    Logger::logFatal("Render World Backend GPU", "Multiple instances special models are not supported");

  size_t index = this->specialAllocations.size() + 1;
  this->specialAllocations.push_back(allocation);
  this->specialPositions.push_back(glm::vec3(0.0));

  size_t id = entity.id;
  if (id >= this->specialIndices.size())
    this->specialIndices.resize(id + 1);

  this->specialIndices[id] = index;
}

void RenderWorldBackendGPU::initEntityQueue(RenderQueue &queue, InstanceManager &manager, const RenderDatabaseView &database, const Entity entity)
{
  RenderWorldBackend::initEntityQueue(queue, manager, database, entity);
  this->initSpecialModel(queue, manager, database, entity);
}

void RenderWorldBackendGPU::updateSpecialPositions(CommandQueue &queue, InstanceManager &manager)
{
  cl_mem fullBufferID = manager.getFullInstancesBuffer().get();
  queue.enqueueAcquireGLBuffer(fullBufferID);

  for (size_t i = 0; i < this->specialPositions.size(); i++)
  {
    Range allocation = this->specialAllocations[i];
    queue.enqueueReadBuffer(fullBufferID, CL_TRUE, allocation.begin * sizeof(InstanceModelMatrixParts) + offsetof(InstanceModelMatrixParts, position), sizeof(InstanceModelMatrixParts{}.position), &this->specialPositions[i]);
  }

  queue.enqueueReleaseGLBuffer(fullBufferID);
}

size_t RenderWorldBackendGPU::getSpecialIndex(const Entity entity)
{
  if (entity.id >= this->specialIndices.size())
    Logger::logFatal("Render World Backend GPU", "Entity ID is out of range");

  size_t idx = this->specialIndices[entity.id];
  if (idx == 0)
    Logger::logFatal("Render World Backend GPU", "Entity index was not initialized before");

  return idx - 1;
}

// Constructor
RenderWorldBackendGPU::RenderWorldBackendGPU(ResourceManager &manager, CommandQueue &queue, Context &ctx, LODGPUBuffers &lodData, BackendGPUBuffers &data, Total &total, size_t modelCount) : lodManagerGPU(manager), renderQueueBuilderGPU(manager), total(total), queue(queue)
{
  if (total.total == 0)
    Logger::logWarning("Render World Backend GPU", "Backend is initialized with total 0");

  if (modelCount == 0)
    Logger::logWarning("Render World Backend GPU", "Backend is initialized with model size 0");

  this->isDouble = ctx.getSupportsDouble();

  this->lodManagerGPU.init(ctx, queue, lodData, this->lodSettings, this->total.total);

  RenderQueueGPUBuffers renderBuffers{
      data,
      LODBuffersData{
          this->lodManagerGPU.getIsFullBuffer(),
          this->lodManagerGPU.getIsNonFullBuffer(),
          this->lodManagerGPU.getIsImpostorBuffer(),
          this->lodManagerGPU.getIsPointBuffer(),
          this->lodManagerGPU.getFullOffsetBuffer(),
          this->lodManagerGPU.getNonFullOffsetBuffer(),
          this->lodManagerGPU.getImpostorOffsetBuffer(),
          this->lodManagerGPU.getPointOffsetBuffer()}};
  this->renderQueueBuilderGPU.init(ctx, queue, renderBuffers, this->lodSettings, modelCount);
}

// Public functions
void RenderWorldBackendGPU::update(RenderQueue &queue, const RenderDatabaseView &database, InstanceManager &instanceManager, FrameContext &ctx)
{
  if (this->lastEntityCount != database.getEntitiesCount() && this->wasSubInit)
    Logger::logFatal("Render World Backend GPU", "GPU is not prepared for dynamic models");

  if (!this->wasSubInit)
  {
    this->initSubQueues(queue, instanceManager, database);
    this->wasSubInit = true;
  }

  if (this->isDouble)
    this->renderQueueBuilderGPU.build<double>(this->queue, queue, this->lodManagerGPU, instanceManager, database.getCamera(), ctx, database.getModels(), this->total.total);
  else
    this->renderQueueBuilderGPU.build<float>(this->queue, queue, this->lodManagerGPU, instanceManager, database.getCamera(), ctx, database.getModels(), this->total.total);

  this->updateSpecialPositions(this->queue, instanceManager);
}

void RenderWorldBackendGPU::sync(IPhysicsWorld &physics, const RenderDatabaseView &database, PointLight *light)
{
  std::cout << "SUN POS: " << this->specialPositions[this->getSpecialIndex(physics.getSun())].x << " " << this->specialPositions[this->getSpecialIndex(physics.getSun())].y << " " << this->specialPositions[this->getSpecialIndex(physics.getSun())].z << std::endl;
  if (light)
    light->move(this->specialPositions[this->getSpecialIndex(physics.getSun())]);
  else
    Logger::logFatal("Scene", " No sun light to sync position");

  // database.forEachSpecialEntity([](const std::unique_ptr<Entity> &entity) {

  // });

  // todo update reflector pos
  // database.getRender.getRelfectanceAcceptorEntities
  // update their pos internally

  // database.getTrails
  // Trail.entity -> getSpecialIdx
  // trail.setPos
}
