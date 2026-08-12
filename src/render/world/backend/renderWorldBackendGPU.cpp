#include "render/world/backend/renderWorldBackendGPU.h"

#include "debug/logger.h"

#include "render/world/backend/backendGPUData.h"
#include "render/queue/data/renderQueueGPUData.h"

#include "compute/context.h"

#include "physics/world/total.h"

// Constructor
RenderWorldBackendGPU::RenderWorldBackendGPU(ResourceManager &manager, CommandQueue &queue, Context &ctx, LODGPUData &lodData, BackendGPUData &data, Total &total, std::vector<Model *> &models) : models(models), lodManagerGPU(manager), renderQueueBuilderGPU(manager), total(total), queue(queue)
{
  if (total.total == 0)
    Logger::logWarning("Render World Backend GPU", "Backend is initialized with total 0");

  if (models.size() == 0)
    Logger::logWarning("Render World Backend GPU", "Backend is initialized with model size 0");

  this->isDouble = ctx.getSupportsDouble();

  this->lodManagerGPU.init(ctx, queue, lodData, this->lodSettings, this->total.total);

  RenderQueueGPUData renderData{
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
  this->renderQueueBuilderGPU.init(ctx, queue, renderData, this->lodSettings, this->models.size());
}

// Public functions
void RenderWorldBackendGPU::update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx)
{
  if (!this->subQueuesInitialized)
    this->initSubQueues(queue, instanceManager, this->models);

  if (this->isDouble)
    this->renderQueueBuilderGPU.build<double>(this->queue, queue, this->lodManagerGPU, instanceManager, camera, ctx, this->models, this->total.total);
  else
    this->renderQueueBuilderGPU.build<float>(this->queue, queue, this->lodManagerGPU, instanceManager, camera, ctx, this->models, this->total.total);
}
