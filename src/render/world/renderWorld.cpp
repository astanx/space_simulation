#include "render/world/renderWorld.h"

#include "render/world/data/renderDataGPU.h"

#include "render/queue/builder/renderQueueBuilder.h"
#include "render/modelSource.h"
#include "render/updatable.h"
#include "render/renderSystem.h"

#include "scene/scene.h"

#include "graphics/model.h"

// Private functions
void RenderWorld::buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx)
{
  // change
  bool GPU = true;
  if (GPU)
    this->renderQueueBuilderGPU->build(this->queue, queue, *this->lodManagerGPU, this->instanceManager, camera, ctx, this->models, this->total.total);
  else
  {
    std::vector<Model *> models;
    size_t index = 0;
    for (ModelSource *source : this->modelSources)
      source->forEachModel([&models, &index](Model &model)
                           { models.push_back(&model); });

    RenderQueueBuilder builder(models);
    builder.build(queue, camera, this->modelSources, this->renderSystems, this->lodManager, this->instanceManager, ctx);
  }
}

void RenderWorld::reserveModelInstances()
{
  // change later
  bool GPU = true;
  if (!GPU)
  {
    for (ModelSource *source : this->modelSources)
      source->reserveInstances(this->instanceManager);

    for (RenderSystem *system : this->renderSystems)
      system->reserveInstances(this->instanceManager);
  }
}

// Public functions
void RenderWorld::init()
{
  this->reserveModelInstances();
  this->lodManager.init(this->modelSources, this->renderSystems);
  this->instanceManager.init(this->total.total);
}

void RenderWorld::initGPU(Context &ctx, RenderDataGPU &gpu)
{
  cl_context context = ctx.get();

  this->modelColorsBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelColors.size() * sizeof(Vec3<float>), gpu.modelColors.data());
  this->modelImportancesBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelImportances.size() * sizeof(float), gpu.modelImportances.data());
  this->modelTextureLayersBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelTextureLayers.size() * sizeof(uint32_t), gpu.modelTextureLayers.data());
  this->modelRangeStartBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelRangeStart.size() * sizeof(uint32_t), gpu.modelRangeStart.data());
  this->modelRangeEndBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelRangeEnd.size() * sizeof(uint32_t), gpu.modelRangeEnd.data());
  this->isNonFullableBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.isNonFullable.size() * sizeof(uint32_t), gpu.isNonFullable.data());
  this->modelFullCountBuffer.init(context, CL_MEM_READ_WRITE, gpu.modelRangeEnd.size() * sizeof(uint32_t), nullptr);
  this->rangeCount = gpu.modelRangeStart.size();
  this->models = std::move(gpu.models);

  this->instanceManager.initGPU(context);
}

void RenderWorld::initRenderQueueGPU(Context &ctx, ResourceManager &resourceManager, SharedGPUData &data)
{
  LODGPUData lodData{data.positionsBuffer,
                     data.meanRadiiBuffer,
                     this->modelImportancesBuffer,
                     this->modelRangeStartBuffer,
                     this->modelRangeEndBuffer,
                     this->isNonFullableBuffer,
                     this->rangeCount};

  this->lodManagerGPU = std::make_unique<LODManagerGPU>(resourceManager);
  this->lodManagerGPU->init(ctx, this->queue, lodData, this->lodSettings, this->total.total);

  RenderQueueGPUData queueData{
      data.positionsBuffer,
      data.orientationsBuffer,
      data.meanRadiiBuffer,
      data.polarRadiiBuffer,
      data.equatorianRadiiBuffer,
      this->modelImportancesBuffer,
      this->modelColorsBuffer,
      this->modelTextureLayersBuffer,
      this->instanceManager.getFullInstancesBuffer(),
      this->instanceManager.getImpostorInstancesBuffer(),
      this->instanceManager.getPointInstancesBuffer(),
      this->modelRangeStartBuffer,
      this->modelRangeEndBuffer,
      this->lodManagerGPU->getIsFullBuffer(),
      this->lodManagerGPU->getIsNonFullBuffer(),
      this->lodManagerGPU->getIsImpostorBuffer(),
      this->lodManagerGPU->getIsPointBuffer(),
      this->lodManagerGPU->getFullOffsetBuffer(),
      this->lodManagerGPU->getNonFullOffsetBuffer(),
      this->lodManagerGPU->getImpostorOffsetBuffer(),
      this->lodManagerGPU->getPointOffsetBuffer(),
      this->rangeCount};

  this->renderQueueBuilderGPU = std::make_unique<RenderQueueBuilderGPU>(resourceManager);
  this->renderQueueBuilderGPU->init(ctx, this->queue, queueData, lodSettings, this->models.size());
}

void RenderWorld::update(const Camera &camera, RenderQueue &queue, FrameContext &ctx)
{
  for (Updatable *&object : this->updatable)
    object->update(camera);

  for (std::unique_ptr<Trail> &trail : this->trails)
    trail->update(camera);

  this->buildQueue(camera, queue, ctx);
}

void RenderWorld::renderImpostorMeshInstanced()
{
  this->lodManager.getImpostorMesh().renderInstanced(&this->instanceManager.getImpostorInstancesVBO(), sizeof(InstancePositionRadiusTexture), this->instanceManager.getImpostorCount());
}

void RenderWorld::renderPointMeshInstanced()
{
  this->lodManager.getPointMesh().renderInstanced(&this->instanceManager.getPointInstancesVBO(), sizeof(InstancePositionRadiusColor), this->instanceManager.getPointCount());
}

void RenderWorld::addRenderSystem(RenderSystem *system)
{
  this->renderSystems.push_back(system);
}

void RenderWorld::addModelSource(ModelSource *object)
{
  this->modelSources.push_back(object);
}

void RenderWorld::addUpdatable(Updatable *object)
{
  this->updatable.push_back(object);
}

void RenderWorld::addTrail(std::unique_ptr<Trail> trail)
{
  this->trailViews.push_back(trail.get());
  this->trails.push_back(std::move(trail));
}

std::vector<Trail *> &RenderWorld::getTrails()
{
  if (this->trails.empty())
    Logger::logWarning("RenderWorld", "Trails are empty");

  return this->trailViews;
};