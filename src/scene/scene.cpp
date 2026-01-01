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
  Texture *diff = this->resourceManager->GetTexture(Res::CONTAINER_DIFFUSE);
  Texture *spec = this->resourceManager->GetTexture(Res::CONTAINER_SPECULAR);
  Material *mat = this->resourceManager->LoadMaterial(Res::CONTAINER, glm::vec3(0.1f),
                                                      glm::vec3(0.9f, 0.5f, 0.4f),
                                                      glm::vec3(0.3f),
                                                      diff, spec, 32.0f);
  Mesh *cubeMesh = this->resourceManager->GetMesh(Res::CUBE_MESH);
  Mesh *planeMesh = this->resourceManager->GetMesh(Res::PLANE_MESH);
  Mesh *sphereMesh = this->resourceManager->GetMesh(Res::SPHERE_MESH);

  std::vector<Mesh *> meshes1;
  meshes1.push_back(cubeMesh);

  std::vector<Mesh *> meshes2;
  meshes2.push_back(planeMesh);

  std::vector<Mesh *> meshes3;
  meshes3.push_back(sphereMesh);

  auto container = std::make_unique<Model>(glm::vec3(-3.0f, 0.0f, 0.0f), mat, meshes1, diff, spec);
  auto plane = std::make_unique<Model>(glm::vec3(0.0f, -2.0f, 0.0f), mat, meshes2, diff, spec, glm::vec3(0.f), glm::vec3(100.f));

  Material *sphereMat = this->resourceManager->LoadMaterial(Res::SPHERE_MATERIAL,
                                                            glm::vec3(0.1f),
                                                            glm::vec3(0.7f, 0.3f, 0.5f),
                                                            glm::vec3(0.3f),
                                                            nullptr, nullptr, 32.f);
  auto sphere = std::make_unique<Model>(glm::vec3(3.0f, 0.0f, 0.0f), sphereMat, meshes3);

  Texture *diff_backpack = this->resourceManager->GetTexture(Res::BACKPACK_DIFFUSE);
  Texture *spec_backpack = this->resourceManager->GetTexture(Res::BACKPACK_SPECULAR);
  Material *mat_backpack = this->resourceManager->LoadMaterial(Res::BACKPACK, glm::vec3(0.1f),
                                                               glm::vec3(0.9f, 0.5f, 0.4f),
                                                               glm::vec3(0.3f),
                                                               diff_backpack, spec_backpack, 32.0f);

  auto backpack = std::make_unique<Model>(glm::vec3(0.0f, 0.0f, 0.0f), mat_backpack, "assets/models/backpack.obj");
  this->addModel(std::move(container));
  this->addModel(std::move(plane));
  this->addModel(std::move(sphere));
  this->addModel(std::move(backpack));

  auto pointLight = std::make_unique<PointLight>(
      glm::vec3(1.2f, 1.0f, 2.0f),
      glm::vec3(0.5f),
      glm::vec3(1.0f),
      glm::vec3(1.0f),
      1.f,
      1.f,
      0.09f,
      0.032f);

  this->addPointLight(std::move(pointLight));

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
