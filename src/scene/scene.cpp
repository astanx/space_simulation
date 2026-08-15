#include "scene/scene.h"

#include "camera/camera.h"

// Process functions
void Scene::init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime)
{
  this->world.init(renderCtx, resourceManager, threadPool, startTime);
}

void Scene::processKeyboard(CameraMovement direction, float deltaTime)
{
  Camera &camera = this->world.getRenderWorld().getActiveCamera();
  camera.processKeyboard(direction, deltaTime);
}

void Scene::processMouseMovement(const float &xpos, const float &ypos)
{
  Camera &camera = this->world.getRenderWorld().getActiveCamera();
  camera.processMouseMovement(xpos, ypos);
}

void Scene::processMouseScroll(float yoffset)
{
  Camera &camera = this->world.getRenderWorld().getActiveCamera();
  camera.processMouseScroll(yoffset);
}

void Scene::update(RenderQueue &queue, RenderContext &renderCtx)
{
  this->world.update(this->getActiveCamera(), queue, renderCtx);
}

// Setters
void Scene::increaseCameraSpeed(double percentage)
{
  Camera &camera = this->world.getRenderWorld().getActiveCamera();
  camera.increaseMovementSpeed(percentage);
}

void Scene::decreaseCameraSpeed(double percentage)
{
  Camera &camera = this->world.getRenderWorld().getActiveCamera();
  camera.decreaseMovementSpeed(percentage);
}

// Getters
const Camera &Scene::getActiveCamera()
{
  return this->world.getRenderWorld().getActiveCamera();
};
const Skybox &Scene::getActiveSkybox()
{
  return this->world.getRenderWorld().getActiveSkybox();
};
const glm::vec3 Scene::getActiveCameraPosition()
{
  return this->world.getRenderWorld().getActiveCamera().getPosition();
};

const PointLight *Scene::getPointLight()
{
  return this->world.getRenderWorld().getPointLight();
};
const DirectionalLight *Scene::getDirLight()
{
  return this->world.getRenderWorld().getDirLight();
};
SimulationWorld &Scene::getSimulationWorld()
{
  return this->world;
}