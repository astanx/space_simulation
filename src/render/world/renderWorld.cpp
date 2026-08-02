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
  std::vector<Model *> models;
  size_t index = 0;
  for (ModelSource *source : this->modelSources)
    source->forEachModel([&models, &index](Model &model)
                         { models.push_back(&model); });

  RenderQueueBuilder builder(models);

  builder.build(queue, camera, this->modelSources, this->renderSystems, this->lodManager, this->instanceManager, ctx);
}

void RenderWorld::reserveModelInstances()
{
  // change later
  bool GPU = false;
  if (!GPU)
  {
    for (ModelSource *source : this->modelSources)
      source->reserveInstances(this->instanceManager);

    for (RenderSystem *system : this->renderSystems)
      system->reserveInstances(this->instanceManager);
  }
}

// Public functions
void RenderWorld::init(size_t totalObjects)
{
  this->reserveModelInstances();
  this->lodManager.init(this->modelSources, this->renderSystems);
  this->instanceManager.init(totalObjects);
}

void RenderWorld::initGPU(Context &ctx, RenderDataGPU &gpu)
{
  cl_context context = ctx.get();

  this->instanceColorsBuffer.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.instanceColors.size() * sizeof(glm::vec3), gpu.instanceColors.data());
  this->instanceImportancesBuffer.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.instanceImportances.size() * sizeof(float), gpu.instanceImportances.data());
  this->instanceTextureLayersBuffer.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.instanceTextureLayers.size() * sizeof(uint32_t), gpu.instanceTextureLayers.data());
  this->modelRangeStart.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.modelRangeStart.size() * sizeof(uint32_t), gpu.modelRangeStart.data());
  this->modelRangeEnd.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.modelRangeEnd.size() * sizeof(uint32_t), gpu.modelRangeEnd.data());
  this->modelFullCount.init(context, CL_MEM_READ_WRITE, gpu.modelRangeEnd.size() * sizeof(uint32_t), nullptr);
  this->rangeCount = gpu.modelRangeStart.size();
}

void RenderWorld::initLODGPU(Context &ctx, ResourceManager &resourceManager, SharedGPUData &data, size_t totalObjects)
{
  LODGPUData lodData{
      data.positionsBuffer,
      data.orientationsBuffer,
      data.meanRadiiBuffer,
      data.polarRadiiBuffer,
      data.equatorianRadiiBuffer,
      this->instanceImportancesBuffer,
      this->instanceColorsBuffer,
      this->instanceTextureLayersBuffer,
      this->instanceManager.getFullInstancesBuffer(),
      this->instanceManager.getImpostorInstancesBuffer(),
      this->instanceManager.getPointInstancesBuffer(),
      this->modelRangeStart,
      this->modelRangeEnd,
      this->modelFullCount,

      this->rangeCount};

  this->lodManagerGPU = std::make_unique<LODManagerGPU>(resourceManager);
  this->lodManagerGPU->init(ctx, this->queue, lodData, this->lodSettings, totalObjects);
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