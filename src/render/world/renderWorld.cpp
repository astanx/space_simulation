#include "render/world/renderWorld.h"

#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"
#include "scene/world/data/sharedGPUData.h"

#include "render/world/backend/backendGPUData.h"
#include "render/world/backend/renderWorldBackendCPU.h"
#include "render/world/backend/renderWorldBackendGPU.h"

#include "render/updatable.h"
#include "render/renderContext.h"
#include "render/world/data/renderDataGPU.h"

#include "graphics/skybox.h"

#include "compute/context.h"

#include "physics/world/physicsWorld.h"
#include "physics/world/total.h"
#include "physics/trail.h"
#include "physics/star.h"

// Constructor / Destructor
RenderWorld::RenderWorld() = default;
RenderWorld::~RenderWorld() = default;

// Public functions
void RenderWorld::init(ResourceManager &manager, PhysicsWorld &physics, RenderContext &ctx, Total &total)
{
  this->lodResourceManager.init(this->modelSources, this->renderSystems);
  this->instanceManager.init(total.total);

  const Star &sun = physics.getSun();

  this->addPointLight(std::make_unique<PointLight>(sun.getRenderPosition(), glm::vec3(1.0f), sun.getLuminosity(), sun.getRadius()));
  this->addCamera(std::make_unique<Camera>(sun.getRenderPosition(), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), ctx.frameCtx.width, ctx.frameCtx.height));
  this->addSkybox(std::make_unique<Skybox>("assets/skybox/starmap.exr", manager));
}

void RenderWorld::initGPUBuffers(Context &ctx, RenderDataGPU &gpu)
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
}

void RenderWorld::initCPUBackend()
{
  this->backend = std::make_unique<RenderWorldBackendCPU>(this->instanceManager, this->modelSources, this->renderSystems);
}
void RenderWorld::initGPUBackend(Context &ctx, CommandQueue &queue, Total &total, ResourceManager &resourceManager, SharedGPUData &data)
{
  this->instanceManager.initGPU(ctx.get());

  LODGPUData lodData{data.positionsBuffer,
                     data.meanRadiiBuffer,
                     this->modelImportancesBuffer,
                     this->modelRangeStartBuffer,
                     this->modelRangeEndBuffer,
                     this->isNonFullableBuffer,
                     this->rangeCount};

  BackendGPUData backendData{
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
      this->rangeCount};

  this->backend = std::make_unique<RenderWorldBackendGPU>(resourceManager, queue, ctx, lodData, backendData, total, this->models);
}

void RenderWorld::update(const Camera &camera, RenderQueue &queue, FrameContext &ctx)
{
  for (Updatable *&object : this->updatable)
    object->update(camera);

  for (std::unique_ptr<Trail> &trail : this->trails)
    trail->update(camera);

  this->backend->update(camera, queue, this->instanceManager, ctx);
}

void RenderWorld::sync(PhysicsWorld &physics)
{
  this->backend->sync(physics, this->pointLight.get());
}

void RenderWorld::renderImpostorMeshInstanced()
{
  this->lodResourceManager.getImpostorMesh().renderInstanced(&this->instanceManager.getImpostorInstancesVBO(), sizeof(InstancePositionRadiusTexture), this->instanceManager.getImpostorCount());
}

void RenderWorld::renderPointMeshInstanced()
{
  this->lodResourceManager.getPointMesh().renderInstanced(&this->instanceManager.getPointInstancesVBO(), sizeof(InstancePositionRadiusColor), this->instanceManager.getPointCount());
}

// Setters
void RenderWorld::addCamera(std::unique_ptr<Camera> camera)
{
  if (!this->activeCamera)
    this->activeCamera = camera.get();
  this->cameras.push_back(std::move(camera));
}

void RenderWorld::addPointLight(std::unique_ptr<PointLight> pointLight)
{
  this->pointLight = std::move(pointLight);
}

void RenderWorld::addDirLight(std::unique_ptr<DirectionalLight> directionalLight)
{
  this->directionalLight = std::move(directionalLight);
}
void RenderWorld::addSkybox(std::unique_ptr<Skybox> skybox)
{
  if (!this->skybox)
    this->skybox = skybox.get();
  this->skyboxesViews.push_back(skybox.get());
  this->skyboxes.push_back(std::move(skybox));
}

void RenderWorld::increaseCameraSpeed(double percentage)
{
  if (this->activeCamera)
    this->activeCamera->increaseMovementSpeed(percentage);
  else
    Logger::logWarning("Render World", "No active camera to increase speed");
}

void RenderWorld::decreaseCameraSpeed(double percentage)
{
  if (this->activeCamera)
    this->activeCamera->decreaseMovementSpeed(percentage);
  else
    Logger::logWarning("Render World", "No active camera to decrease speed");
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

// Getters
Camera &RenderWorld::getActiveCamera()
{
  if (!this->activeCamera)
    Logger::logFatal("Render World", "No active camera");

  return *this->activeCamera;
};
const Skybox &RenderWorld::getActiveSkybox() const
{
  if (!this->skybox)
    Logger::logFatal("Render World", "No active skybox");

  return *this->skybox;
};
const glm::vec3 RenderWorld::getActiveCameraPosition() const
{
  if (!this->activeCamera)
    Logger::logFatal("Render World", "No active camera, can not get position");

  return this->activeCamera->getPosition();
};

const PointLight *RenderWorld::getPointLight() const
{
  if (this->pointLight)
    Logger::logWarning("Render World", "No point light");

  return this->pointLight.get();
};
const DirectionalLight *RenderWorld::getDirLight() const
{
  if (!this->directionalLight)
    Logger::logWarning("Render World", "No directional light");

  return this->directionalLight.get();
};

std::vector<Trail *> &RenderWorld::getTrails()
{
  if (this->trails.empty())
    Logger::logWarning("Render World", "Trails are empty");

  return this->trailViews;
};