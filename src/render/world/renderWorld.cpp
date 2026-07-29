#include "render/world/renderWorld.h"

#include "render/world/data/renderDataGPU.h"

#include "render/queue/builder/renderQueueBuilder.h"
#include "render/modelSource.h"
#include "render/updatable.h"

#include "scene/scene.h"

#include "graphics/model.h"

// Private functions
void RenderWorld::buildQueue(const Camera &camera, RenderQueue &queue, FrameContext &ctx)
{
  std::vector<Model *> models;
  size_t index = 0;
  for (ModelSource *source : this->modelSources)
    source->forEachModel([&models, &index](Model &model)
                         { 
                          model.setQueueIndex(index++);
                          models.push_back(&model); });

  RenderQueueBuilder builder(models);

  builder.build(queue, camera, this->modelSources, this->renderSystems, this->lodManager, this->instanceManager, ctx);
}

// Public functions
void RenderWorld::init()
{
  this->lodManager.init(this->modelSources, this->renderSystems);
  this->instanceManager.init();
}

void RenderWorld::initGPU(Context &ctx, RenderDataGPU &gpu)
{
  cl_context context = ctx.get();

  this->instanceColorsBuffer.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.instanceColors.size() * sizeof(glm::vec3), gpu.instanceColors.data());
  this->instanceImportancesBuffer.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.instanceImportances.size() * sizeof(float), gpu.instanceImportances.data());
  this->instanceTextureLayersBuffer.init(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, gpu.instanceTextureLayers.size() * sizeof(uint32_t), gpu.instanceTextureLayers.data());
}

void RenderWorld::initLODGPU(Context &ctx, LODGPUData &data, LODSettings &settings, size_t totalObjects)
{
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

void RenderWorld::addRenderable(Renderable *object)
{
  this->renderable.push_back(object);
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

std::vector<Renderable *> &RenderWorld::getRenderable()
{
  if (this->renderable.empty())
    Logger::logWarning("RenderWorld", "Renderable is empty");

  return this->renderable;
};
std::vector<RenderSystem *> &RenderWorld::getRenderSystems()
{
  if (this->renderSystems.empty())
    Logger::logWarning("RenderWorld", "Render systems are empty");

  return this->renderSystems;
};
std::vector<ModelSource *> &RenderWorld::getModelSources()
{
  if (this->modelSources.empty())
    Logger::logWarning("RenderWorld", "Model sources are empty");

  return this->modelSources;
};

std::vector<Trail *> &RenderWorld::getTrails()
{
  if (this->trails.empty())
    Logger::logWarning("RenderWorld", "Trails are empty");

  return this->trailViews;
};