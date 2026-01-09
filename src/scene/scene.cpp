#include "scene/scene.h"
#include "graphics/shader.h"
#include "graphics/primitives/cube.h"
#include "graphics/mesh.h"
#include "physics/planet.h"
#include "physics/orbit.h"
#include "physics/star.h"
#include "physics/moon.h"
#include "physics/constants.h"
#include "resources/resourceManager.h"

#include <iostream>

// Constructor/Destructor
Scene::Scene(ResourceManager *resourceManager)
{
  this->resourceManager = resourceManager;
  this->activeCamera = nullptr;
  this->skybox = nullptr;
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

Planet *Scene::createPlanet(std::string name, std::string material_name, double mu,
                            double radius, Object *centralBody, const KeplerElements keplerElements)
{
  Mesh *mesh = this->resourceManager->GetMesh(name);
  Material *mat = this->resourceManager->GetMaterial(material_name);
  auto model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radius, keplerElements);

  planet->addModel(std::move(model));

  Planet *ptr = planet.get();
  this->addObject(std::move(planet));
  return ptr;
}

Star *Scene::createStar(std::string name, std::string material_name, double mu,
                        double radius, glm::dvec3 position, glm::dvec3 velocity)
{
  Mesh *mesh = this->resourceManager->GetMesh(name);
  Material *mat = this->resourceManager->GetMaterial(material_name);
  auto model = std::make_unique<Model>(position, mat, mesh);

  std::unique_ptr<Star> star = std::make_unique<Star>(mu, radius, position, velocity);

  star->addModel(std::move(model));

  Star *ptr = star.get();
  this->addObject(std::move(star));
  return ptr;
}

Moon *Scene::createMoon(std::string name, std::string material_name, double mu,
                        double radius, Planet *centralBody, const KeplerElements keplerElements)
{
  Mesh *mesh = this->resourceManager->GetMesh(name);
  Material *mat = this->resourceManager->GetMaterial(material_name);
  auto model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radius, keplerElements);

  moon->addModel(std::move(model));

  Moon *ptr = moon.get();

  centralBody->addMoon(std::move(moon));

  return ptr;
}

// Process functions
void Scene::init(float width, float height)
{
  Star *sunPtr = createStar(Res::SUN, Res::SUN_MATERIAL, sunMu, sunRadius, sunPos);
  createPlanet(Res::MERCURY, Res::MERCURY_MATERIAL, mercuryMu, mercuryRadius, sunPtr, mercuryElements);
  createPlanet(Res::VENUS, Res::VENUS_MATERIAL, venusMu, venusRadius, sunPtr, venusElements);
  Planet *earthPtr = createPlanet(Res::EARTH, Res::EARTH_MATERIAL, earthMu, earthRadius, sunPtr, earthElements);
  createMoon(Res::MOON, Res::MOON_MATERIAL, moonMu, moonRadius, earthPtr, moonElements);

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

  std::vector<const char *> faces =
      {

          "assets/skybox/right.png",
          "assets/skybox/left.png",
          "assets/skybox/top.png",
          "assets/skybox/bottom.png",
          "assets/skybox/front.png",
          "assets/skybox/back.png"};

  auto sb = std::make_unique<Skybox>(faces);
  this->addSkybox(std::move(sb));
  this->skybox = this->skyboxes.back().get();
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
void Scene::renderSkybox(Shader *skyboxShader, float aspectRatio)
{
  skyboxShader->use();
  glm::mat4 view = glm::mat4(glm::mat3(this->activeCamera->getViewMatrix()));
  glm::mat4 projection = this->activeCamera->getProjectionMatrix(aspectRatio);

  skyboxShader->setMat4fv(view, "ViewMatrix");
  skyboxShader->setMat4fv(projection, "ProjectionMatrix");

  this->skybox->render(skyboxShader);
  skyboxShader->unuse();
}
void Scene::render(Shader *shader, int framebufferWidth, int framebufferHeight, float dt, Shader *skyboxShader)
{
  if (!activeCamera)
    return;

  float aspect = 1.0f;
  if (framebufferHeight > 0)
    aspect = static_cast<float>(framebufferWidth) / framebufferHeight;
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

  // Render skybox
  renderSkybox(skyboxShader, aspect);
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
void Scene::addSkybox(std::unique_ptr<Skybox> skybox)
{
  this->skyboxes.push_back(std::move(skybox));
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