#include "render/world/renderWorld.h"

#include "scene/light/pointLight.h"
#include "scene/light/directionalLight.h"
#include "scene/world/data/sharedGPUBuffers.h"
#include "scene/world/data/sharedDatabaseView.h"

#include "render/world/backend/backendGPUBuffers.h"
#include "render/world/backend/renderWorldBackendCPU.h"
#include "render/world/backend/renderWorldBackendGPU.h"

#include "render/updatable.h"
#include "render/renderContext.h"
#include "render/world/data/renderDatabase.h"
#include "render/world/data/renderDatabaseView.h"

#include "graphics/skybox.h"

#include "compute/context.h"

#include "physics/world/total.h"
#include "physics/trail.h"

// Constructor / Destructor
RenderWorld::RenderWorld() = default;
RenderWorld::~RenderWorld() = default;

// Public functions
void RenderWorld::init(Total &total)
{
  this->lodResourceManager.init(this->models);
  this->instanceManager.init(total.total);
}

void RenderWorld::initGPUBuffers(Context &ctx, RenderDatabase &gpu)
{
  cl_context context = ctx.get();

  this->gpuBuffers.modelColorsBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelColors.size() * sizeof(Vec3<float>), gpu.modelColors.data());
  this->gpuBuffers.modelImportancesBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelImportances.size() * sizeof(float), gpu.modelImportances.data());
  this->gpuBuffers.modelTextureLayersBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelTextureLayers.size() * sizeof(uint32_t), gpu.modelTextureLayers.data());
  this->gpuBuffers.modelRangeStartBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelRangeStart.size() * sizeof(uint32_t), gpu.modelRangeStart.data());
  this->gpuBuffers.modelRangeEndBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.modelRangeEnd.size() * sizeof(uint32_t), gpu.modelRangeEnd.data());
  this->gpuBuffers.isNonFullableBuffer.init(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, gpu.isNonFullable.size() * sizeof(uint32_t), gpu.isNonFullable.data());
  this->gpuBuffers.modelFullCountBuffer.init(context, CL_MEM_READ_WRITE, gpu.modelRangeEnd.size() * sizeof(uint32_t), nullptr);
  this->rangeCount = gpu.modelRangeStart.size();
  this->models = std::move(gpu.models);
}

void RenderWorld::initCPUBackend()
{
  this->backend = std::make_unique<RenderWorldBackendCPU>();
}
void RenderWorld::initGPUBackend(Context &ctx, CommandQueue &queue, Total &total, ResourceManager &resourceManager, SharedGPUBuffers &data)
{
  this->instanceManager.initGPU(ctx.get());

  LODGPUBuffers lodBuffers{data.positionsBuffer,
                           data.meanRadiiBuffer,
                           this->gpuBuffers.modelImportancesBuffer,
                           this->gpuBuffers.modelRangeStartBuffer,
                           this->gpuBuffers.modelRangeEndBuffer,
                           this->gpuBuffers.isNonFullableBuffer,
                           this->rangeCount};

  BackendGPUBuffers backendBuffers{
      data.positionsBuffer,
      data.orientationsBuffer,
      data.meanRadiiBuffer,
      data.polarRadiiBuffer,
      data.equatorianRadiiBuffer,
      this->gpuBuffers.modelImportancesBuffer,
      this->gpuBuffers.modelColorsBuffer,
      this->gpuBuffers.modelTextureLayersBuffer,
      this->instanceManager.getFullInstancesBuffer(),
      this->instanceManager.getImpostorInstancesBuffer(),
      this->instanceManager.getPointInstancesBuffer(),
      this->gpuBuffers.modelRangeStartBuffer,
      this->gpuBuffers.modelRangeEndBuffer,
      this->rangeCount};

  this->backend = std::make_unique<RenderWorldBackendGPU>(resourceManager, queue, ctx, lodBuffers, backendBuffers, total, this->models);
}

void RenderWorld::update(RenderQueue &queue, FrameContext &ctx, const SharedDatabaseView &shared, const EntityManager &entityManager)
{
  // for (Updatable *&object : this->updatable)
  //   object->update(*this->activeCamera);

  for (std::unique_ptr<Trail> &trail : this->trails)
    trail->update(*this->activeCamera);

  RenderDatabaseView view{entityManager, *this->activeCamera, shared, this->database};
  this->backend->update(queue, view, this->instanceManager, ctx);
}

void RenderWorld::sync(IPhysicsWorld &physics, const SharedDatabaseView &shared, const EntityManager &entityManager)
{
  RenderDatabaseView view{entityManager, *this->activeCamera, shared, this->database};
  this->backend->sync(physics, view, this->pointLight.get());
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