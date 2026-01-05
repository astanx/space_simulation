#include "scene/scene.h"
#include "graphics/shader.h"
#include "graphics/primitives/cube.h"
#include "graphics/mesh.h"
#include "physics/planet.h"
#include "physics/orbit.h"
#include "physics/constants.h"
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

  // SUN
  Mesh *sunMesh = this->resourceManager->GetMesh(Res::SUN);
  Material *sunMat = this->resourceManager->GetMaterial(Res::SUN_MATERIAL);
  auto sunModel = std::make_unique<Model>(sunPos * VISUAL_SCALE, sunMat, sunMesh);
  auto sun = std::make_unique<Planet>(sunPos, sunMass, sunRadius, std::move(sunModel));
  Planet *sunPtr = sun.get();
  this->addObject(std::move(sun));

  // MERCURY
  Mesh *mercuryMesh = this->resourceManager->GetMesh(Res::MERCURY);
  Material *mercuryMat = this->resourceManager->GetMaterial(Res::MERCURY_MATERIAL);
  auto mercuryModel = std::make_unique<Model>(mercuryPos * VISUAL_SCALE, mercuryMat, mercuryMesh);
  auto mercuryOrbit = std::make_unique<Orbit>(sunPtr, mercuryOrbitalPeriod, mercuryInclination, mercuryLongitude);
  auto mercury = std::make_unique<Planet>(mercuryPos, mercuryMass, mercuryRadius, std::move(mercuryModel), glm::vec3(0.f), std::move(mercuryOrbit));
  Planet *mercuryPtr = mercury.get();
  this->addObject(std::move(mercury));

  // VENUS
  Mesh *venusMesh = this->resourceManager->GetMesh(Res::VENUS);
  Material *venusMat = this->resourceManager->GetMaterial(Res::VENUS_MATERIAL);
  auto venusModel = std::make_unique<Model>(venusPos * VISUAL_SCALE, venusMat, venusMesh);
  auto venusOrbit = std::make_unique<Orbit>(sunPtr, venusOrbitalPeriod, venusInclination, venusLongitude);
  auto venus = std::make_unique<Planet>(venusPos, venusMass, venusRadius, std::move(venusModel), glm::vec3(0.f), std::move(venusOrbit));
  Planet *venusPtr = venus.get();
  this->addObject(std::move(venus));

  // EARTH
  Mesh *earthMesh = this->resourceManager->GetMesh(Res::EARTH);
  Material *earthMat = this->resourceManager->GetMaterial(Res::EARTH_MATERIAL);
  auto earthModel = std::make_unique<Model>(earthPos * VISUAL_SCALE, earthMat, earthMesh);
  auto earthOrbit = std::make_unique<Orbit>(sunPtr, earthOrbitalPeriod, earthInclination, earthLongitude);
  auto earth = std::make_unique<Planet>(earthPos, earthMass, earthRadius, std::move(earthModel), glm::vec3(0.f), std::move(earthOrbit));
  Planet *earthPtr = earth.get();
  this->addObject(std::move(earth));

  // MOON
  Mesh *moonMesh = this->resourceManager->GetMesh(Res::MOON);
  Material *moonMat = this->resourceManager->GetMaterial(Res::MOON_MATERIAL);
  auto moonModel = std::make_unique<Model>(moonPos * VISUAL_SCALE, moonMat, moonMesh);
  auto moonOrbit = std::make_unique<Orbit>(earthPtr, moonOrbitalPeriod, moonInclination, moonLongitude);
  auto moon = std::make_unique<Planet>(moonPos, moonMass, moonRadius, std::move(moonModel), glm::vec3(0.f), std::move(moonOrbit));
  this->addObject(std::move(moon));

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

void Scene::update(float dt)
{
  dt *= TIME_SCALE;
  for (size_t i = 0; i < objects.size(); ++i)
    for (size_t j = i + 1; j < objects.size(); ++j)
    {
      objects[i]->applyGravitation(*objects[j]);
      objects[j]->applyGravitation(*objects[i]);
    }

  for (auto &object : objects)
  {
    object->update(dt);
  }
}

void Scene::render(Shader *shader, int framebufferWidth, int framebufferHeight, float dt)
{
  if (!activeCamera)
    return;

  float aspect = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
  if (aspect <= 0.0f)
    aspect = 1.0f;

  this->update(dt);

  shader->use();

  // Send camera
  sendCameraToShader(*shader, aspect);
  // Send lights
  sendLightsToShader(*shader);

  // Render all objects
  for (auto &object : this->objects)
  {
    object->render(shader);
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

void Scene::addObject(std::unique_ptr<Object> object)
{
  this->objects.push_back(std::move(object));
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