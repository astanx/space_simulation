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

Planet *Scene::createPlanet(std::string name, std::string material_name, glm::dvec3 pos, double mass,
                            double radius, Planet *centralBody, double orbitalPeriod, double inclination, double longitude)
{
  Mesh *mesh = this->resourceManager->GetMesh(name);
  Material *mat = this->resourceManager->GetMaterial(material_name);
  auto model = std::make_unique<Model>(pos * VISUAL_SCALE, mat, mesh);

  std::unique_ptr<Planet> planet;
  if (centralBody)
  {
    auto orbit = std::make_unique<Orbit>(centralBody, orbitalPeriod, inclination, longitude);
    planet = std::make_unique<Planet>(pos, mass, radius, std::move(model), glm::vec3(0.f), std::move(orbit));
  }
  else
    planet = std::make_unique<Planet>(pos, mass, radius, std::move(model));

  Planet *ptr = planet.get();
  this->addObject(std::move(planet));
  return ptr;
}

// Process functions
void Scene::init(float width, float height)
{
  Planet* sunPtr = createPlanet(Res::SUN, Res::SUN_MATERIAL, sunPos, sunMass, sunRadius);
  createPlanet(Res::MERCURY, Res::MERCURY_MATERIAL, mercuryPos, mercuryMass, mercuryRadius, sunPtr, mercuryOrbitalPeriod, mercuryInclination, mercuryLongitude);
  createPlanet(Res::VENUS, Res::VENUS_MATERIAL, venusPos, venusMass, venusRadius, sunPtr, venusOrbitalPeriod, venusInclination, venusLongitude);
  Planet* earthPtr = createPlanet(Res::EARTH, Res::EARTH_MATERIAL, earthPos, earthMass, earthRadius, sunPtr, earthOrbitalPeriod, earthInclination, earthLongitude);
  createPlanet(Res::MOON, Res::MOON_MATERIAL, moonPos, moonMass, moonRadius, earthPtr, moonOrbitalPeriod, moonInclination, moonLongitude);

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