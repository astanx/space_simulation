#include "render/world/backend/renderWorldBackendCPU.h"

#include "render/queue/builder/renderQueueBuilder.h"

#include "render/modelSource.h"
#include "render/renderSystem.h"

// Constructor
RenderWorldBackendCPU::RenderWorldBackendCPU(InstanceManager &instanceManager, std::vector<ModelSource *> &modelSources, std::vector<RenderSystem *> &renderSystems) : modelSources(modelSources), renderSystems(renderSystems), lodManager(this->lodSettings)
{
  for (ModelSource *source : this->modelSources)
    source->reserveInstances(instanceManager);

  for (RenderSystem *system : this->renderSystems)
    system->reserveInstances(instanceManager);
}

// Public functions
void RenderWorldBackendCPU::update(const Camera &camera, RenderQueue &queue, InstanceManager &instanceManager, FrameContext &ctx)
{
  std::vector<Model *> models;
  size_t index = 0;
  for (ModelSource *source : this->modelSources)
    source->forEachModel([&models, &index](Model &model)
                         { models.push_back(&model); });

  RenderQueueBuilder builder(models);
  builder.build(queue, camera, this->modelSources, this->renderSystems, this->lodManager, instanceManager, ctx);
}