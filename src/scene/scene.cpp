#include "scene/scene.h"

#include "scene/world/simulationWorld.h"

#include "camera/camera.h"

#include "physics/world/IphysicsWorld.h"

#include "render/world/renderWorld.h"

#include "graphics/skybox.h"

// Constructor / Destructor
Scene::Scene() = default;
Scene::~Scene() = default;

// Public functions
void Scene::init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, WorldConfig &cfg)
{
  if (cfg.precision == Precision::DOUBLE)
    this->world.emplace<SimulationWorld<double>>();
  else if (cfg.precision == Precision::FLOAT)
    this->world.emplace<SimulationWorld<float>>();
  else
    Logger::logFatal("Scene", "Unsupported precision");

  std::visit([&renderCtx, &resourceManager, &threadPool, &cfg](auto &w)
             { w.init(renderCtx, resourceManager, threadPool, cfg); }, this->world);
}

// Process functions
void Scene::initGPUWorld(ResourceManager &resourceManager)
{
  std::visit([&resourceManager](auto &w)
             { w.initGPU(resourceManager); }, this->world);
}
void Scene::initCPUWorld(ThreadPool &threadPool)
{
  std::visit([&threadPool](auto &w)
             { w.initCPU(threadPool); }, this->world);
}

void Scene::processKeyboard(CameraMovement direction, float deltaTime)
{
  std::visit([direction, deltaTime](auto &w)
             { 
        Camera &camera = w.getRenderWorld().getActiveCamera();
  camera.processKeyboard(direction, deltaTime); }, this->world);
}

void Scene::processMouseMovement(const float &xpos, const float &ypos)
{
  std::visit([xpos, ypos](auto &w)
             {
               Camera &camera = w.getRenderWorld().getActiveCamera();
               camera.processMouseMovement(xpos, ypos); },
             this->world);
}

void Scene::processMouseScroll(float yoffset)
{
  std::visit([yoffset](auto &w)
             { 
        Camera &camera = w.getRenderWorld().getActiveCamera();
  camera.processMouseScroll(yoffset); }, this->world);
}

void Scene::updatePhysicsWorld(double dt)
{
  std::visit([dt](auto &w)
             { w.updatePhysics(dt); }, this->world);
}

void Scene::updateRenderWorld(RenderQueue &queue, RenderContext &renderCtx)
{
  std::visit([&queue, &renderCtx](auto &w)
             { w.updateRender(queue, renderCtx); }, this->world);
}

// Setters
void Scene::increaseCameraSpeed(double percentage)
{
  std::visit([percentage](auto &w)
             {
               Camera &camera = w.getRenderWorld().getActiveCamera();
               camera.increaseMovementSpeed(percentage); },
             this->world);
}

void Scene::decreaseCameraSpeed(double percentage)
{
  std::visit([percentage](auto &w)
             {
               Camera &camera = w.getRenderWorld().getActiveCamera();
               camera.decreaseMovementSpeed(percentage); },
             this->world);
}

// Getters
const Camera &Scene::getActiveCamera()
{
  return std::visit([](auto &w) -> const Camera &
                    { return w.getRenderWorld().getActiveCamera(); }, this->world);
};
const Skybox &Scene::getActiveSkybox()
{
  return std::visit([](auto &w) -> const Skybox &
                    { return w.getRenderWorld().getActiveSkybox(); }, this->world);
};
const glm::vec3 Scene::getActiveCameraPosition()
{
  return std::visit([](auto &w)
                    { return w.getRenderWorld().getActiveCamera().getPosition(); }, this->world);
};

const PointLight *Scene::getPointLight()
{
  return std::visit([](auto &w)
                    { return w.getRenderWorld().getPointLight(); }, this->world);
};
const DirectionalLight *Scene::getDirLight()
{
  return std::visit([](auto &w)
                    { return w.getRenderWorld().getDirLight(); }, this->world);
};
ISimulationWorld &Scene::getSimulationWorld()
{
  return std::visit([](auto &w) -> ISimulationWorld &
                    { return w; }, this->world);
}