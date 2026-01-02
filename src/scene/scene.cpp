#include "scene/scene.h"
#include "graphics/shader.h"
#include "graphics/primitives/cube.h"
#include "graphics/mesh.h"
#include "resources/resourceManager.h"

#include <iostream>

// Constructor/Destructor
Scene::Scene(ResourceManager *resourceManager)
{
  this->resourceManager = resourceManager;
  this->activeCamera = nullptr;
}

// Public functions
void Scene::sendLightsToShader(Shader &shader)
{
  for (auto &pointLight : this->pointLights)
    pointLight->sendToShader(shader);
  if (this->directionalLight)
    this->directionalLight->sendToShader(shader);
}

void Scene::sendCameraToShader(Shader &shader, float aspectRatio)
{
  if (!this->activeCamera)
    throw "ERROR:SCENE:NO_ACTIVE_CAMERA";

  shader.setMat4fv(this->activeCamera->getProjectionMatrix(aspectRatio), "ProjectionMatrix");
  shader.setMat4fv(this->activeCamera->getViewMatrix(), "ViewMatrix");
  shader.setVec3f(this->activeCamera->getPosition(), "camPosition");
}

// Process functions
void Scene::init(float width, float height)
{
  Mesh *sphereMesh = this->resourceManager->GetMesh(Res::SPHERE_MESH);

  std::vector<Mesh *> earthMeshes;
  earthMeshes.push_back(sphereMesh);
  Material *earthMat = this->resourceManager->GetMaterial(Res::EARTH_MATERIAL);
  auto earth = std::make_unique<Model>(glm::vec3(3.f, 0.f, 0.f), earthMat, earthMeshes);
  this->addModel(std::move(earth));

  std::vector<Mesh *> sunMeshes;
  sunMeshes.push_back(sphereMesh);
  Material *sunMat = this->resourceManager->GetMaterial(Res::SUN_MATERIAL);
  auto sun = std::make_unique<Model>(glm::vec3(0.f, 0.f, 0.f), sunMat, sunMeshes);
  this->addModel(std::move(sun));

  auto pointLight = std::make_unique<PointLight>(
      glm::vec3(0.f, 0.f, 0.f),
      glm::vec3(0.5f),
      glm::vec3(1.0f),
      glm::vec3(1.0f),
      1.f,
      1.f,
      0.09f,
      0.032f);

  this->addPointLight(std::move(pointLight));

  auto dirLight = std::make_unique<DirectionalLight>(
      glm::vec3(1.2f, 1.0f, 2.0f),
      glm::vec3(0.5f),
      glm::vec3(1.0f),
      glm::vec3(1.0f), 1.f);

  this->addDirLight(std::move(dirLight));

  auto cam = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f),
                                      glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f),
                                      width,
                                      height);

  this->addCamera(std::move(cam));
  activeCamera = this->cameras.back().get();
}
void Scene::processKeyboard(CameraMovement direction, float deltaTime)
{
  if (!this->activeCamera)
    throw "ERROR:SCENE:NO_ACTIVE_CAMERA";

  this->activeCamera->processKeyboard(direction, deltaTime);
}

void Scene::processMouseMovement(const float &xpos, const float &ypos)
{
  if (!this->activeCamera)
    throw "ERROR:SCENE:NO_ACTIVE_CAMERA";

  this->activeCamera->processMouseMovement(xpos, ypos);
}

void Scene::processMouseScroll(float yoffset)
{
  if (!this->activeCamera)
    throw "ERROR:SCENE:NO_ACTIVE_CAMERA";

  this->activeCamera->processMouseScroll(yoffset);
}

void Scene::render(Shader *shader, int framebufferWidth, int framebufferHeight)
{
  if (!activeCamera)
    return;

  float aspect = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
  if (aspect <= 0.0f)
    aspect = 1.0f;

  shader->use();

  // Send camera
  sendCameraToShader(*shader, aspect);
  // Send lights
  sendLightsToShader(*shader);

  // Render all models
  int i = 0;
  for (auto &model : this->models)
  {
    model->render(shader);
  }

  shader->unuse();
}

// Setters
void Scene::addCamera(std::unique_ptr<Camera> camera)
{
  if (!this->activeCamera)
    this->activeCamera = camera.get();

  this->cameras.push_back(std::move(camera));
}

void Scene::addModel(std::unique_ptr<Model> model)
{
  this->models.push_back(std::move(model));
}

void Scene::addPointLight(std::unique_ptr<PointLight> pointLight)
{
  this->pointLights.push_back(std::move(pointLight));
}

void Scene::addDirLight(std::unique_ptr<DirectionalLight> directionalLight)
{
  this->directionalLight = std::move(directionalLight);
}

// Getters
Camera &Scene::getActiveCamera()
{
  if (!this->activeCamera)
    throw "ERROR:SCENE:NO_ACTIVE_CAMERA";
  return *this->activeCamera;
}

const glm::vec3 Scene::getActiveCameraPosition() const
{
  if (!this->activeCamera)
    throw "ERROR:SCENE:NO_ACTIVE_CAMERA";
  return this->activeCamera->getPosition();
}

const std::vector<std::unique_ptr<PointLight>> &Scene::getPointLights() const
{
  return this->pointLights;
}

const std::unique_ptr<DirectionalLight> &Scene::getDirLight() const
{
  return this->directionalLight;
}