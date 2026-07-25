#include "scene/scene.h"

#include "debug/logger.h"

#include "physics/star.h"

// Process functions
void Scene::init(RenderContext &renderCtx, ResourceManager &resourceManager, ThreadPool &threadPool, double startTime)
{
  this->world.init(resourceManager, threadPool, startTime);

  const Star &sun = this->world.getPhysicsWorld().getSun();
  std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>(
      sun.getRenderPosition(),
      glm::vec3(1.0f),
      sun.getLuminosity(),
      sun.getRadius());
  this->addPointLight(std::move(pointLight));

  std::unique_ptr<Camera> cam = std::make_unique<Camera>(sun.getRenderPosition(),
                                                         glm::vec3(0.0f, 0.0f, -1.0f),
                                                         glm::vec3(0.0f, 1.0f, 0.0f),
                                                         renderCtx.frameCtx.width, renderCtx.frameCtx.height);
  this->addCamera(std::move(cam));
  activeCamera = this->cameras.back().get();

  std::unique_ptr<Skybox> sb = std::make_unique<Skybox>("assets/skybox/starmap.exr", resourceManager);
  this->addSkybox(std::move(sb));
  this->skybox = this->skyboxes.back().get();
}

void Scene::processKeyboard(CameraMovement direction, float deltaTime)
{
  assert(this->activeCamera && "[Scene] ASSERT: No active camera to process keyboard");

  this->activeCamera->processKeyboard(direction, deltaTime);
}

void Scene::processMouseMovement(const float &xpos, const float &ypos)
{
  assert(this->activeCamera && "[Scene] ASSERT: No active camera to process mouse movement");

  this->activeCamera->processMouseMovement(xpos, ypos);
}

void Scene::processMouseScroll(float yoffset)
{
  assert(this->activeCamera && "[Scene] ASSERT: No active camera to process mouse scroll");

  this->activeCamera->processMouseScroll(yoffset);
}

void Scene::update(RenderContext &renderCtx)
{
  this->world.update(renderCtx, this->getActiveCamera());

  if (this->pointLights[0])
    this->pointLights[0]->move(this->world.getPhysicsWorld().getSun().getRenderPosition()); // move sun light
  else
    Logger::logFatal("Scene", " No sun to update position");
}

// Setters
void Scene::addCamera(std::unique_ptr<Camera> camera)
{
  if (!this->activeCamera)
    this->activeCamera = camera.get();

  this->cameras.push_back(std::move(camera));
}

void Scene::addPointLight(std::unique_ptr<PointLight> pointLight)
{
  this->pointLightViews.push_back(pointLight.get());
  this->pointLights.push_back(std::move(pointLight));
}

void Scene::addDirLight(std::unique_ptr<DirectionalLight> directionalLight)
{
  this->directionalLight = std::move(directionalLight);
}
void Scene::addSkybox(std::unique_ptr<Skybox> skybox)
{
  this->skyboxesViews.push_back(skybox.get());
  this->skyboxes.push_back(std::move(skybox));
}

void Scene::increaseCameraSpeed(double percentage)
{
  if (this->activeCamera)
    this->activeCamera->increaseMovementSpeed(percentage);
  else
    Logger::logWarning("Scene", "No active camera to increase speed");
}

void Scene::decreaseCameraSpeed(double percentage)
{
  if (this->activeCamera)
    this->activeCamera->decreaseMovementSpeed(percentage);
  else
    Logger::logWarning("Scene", "No active camera to decrease speed");
}

// Getters
const Camera &Scene::getActiveCamera() const
{
  if (!this->activeCamera)
    Logger::logFatal("Scene", "No active camera");

  return *this->activeCamera;
};
const Skybox &Scene::getActiveSkybox() const
{
  if (!this->skybox)
    Logger::logFatal("Scene", "No active skybox");

  return *this->skybox;
};
const glm::vec3 Scene::getActiveCameraPosition() const
{
  if (!this->activeCamera)
    Logger::logFatal("Scene", "No active camera, can not get position");

  return this->activeCamera->getPosition();
};

const std::vector<PointLight *> &Scene::getPointLights() const
{
  if (this->pointLightViews.empty())
    Logger::logWarning("Scene", "Point light views are empty");

  return this->pointLightViews;
};
const DirectionalLight *Scene::getDirLight() const
{
  if (!this->directionalLight)
    Logger::logWarning("Scene", "No directional light");

  return this->directionalLight.get();
};
SimulationWorld &Scene::getSimulationWorld()
{
  return this->world;
}