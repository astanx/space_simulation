#include "scene/scene.h"
#include "graphics/shader.h"
#include "graphics/primitives/sphere.h"
#include "graphics/mesh.h"
#include "physics/planet.h"
#include "physics/orbit.h"
#include "physics/star.h"
#include "physics/moon.h"
#include "physics/constants.h"
#include "resources/resourceManager.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void Scene::initShaderBuffer(GLuint *ubo, unsigned long size, GLenum bufferType)
{
  glGenBuffers(1, ubo);
  glBindBuffer(bufferType, *ubo);
  glBufferData(
      bufferType,
      size,
      nullptr,
      GL_DYNAMIC_DRAW);
}

// Constructor/Destructor
Scene::Scene(ResourceManager &resourceManager, ThreadPool &threadPool) : threadPool(threadPool), resourceManager(resourceManager)
{
  this->activeCamera = nullptr;
  this->skybox = nullptr;
}

// Public functions
Planet *Scene::createPlanet(std::string name, std::string material_name, double mu,
                            double radius, Object *centralBody, const KeplerElements keplerElements)
{
  Mesh *mesh = this->resourceManager.GetMesh(name);
  Material *mat = this->resourceManager.GetMaterial(material_name);
  auto model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Planet> planet = std::make_unique<Planet>(centralBody, mu, radius, keplerElements);

  planet->addModel(std::move(model));

  Planet *ptr = planet.get();
  if (planet->getUseTrail())
    this->addTrail(planet->generateTrail());
  this->addObject(std::move(planet));
  return ptr;
}

Star *Scene::createStar(std::string name, std::string material_name, double mu,
                        double radius, glm::dvec3 position, glm::dvec3 velocity)
{
  Mesh *mesh = this->resourceManager.GetMesh(name);
  Material *mat = this->resourceManager.GetMaterial(material_name);
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
  Mesh *mesh = this->resourceManager.GetMesh(name);
  Material *mat = this->resourceManager.GetMaterial(material_name);
  auto model = std::make_unique<Model>(glm::dvec3(0.0), mat, mesh);

  std::unique_ptr<Moon> moon = std::make_unique<Moon>(centralBody, mu, radius, keplerElements);

  moon->addModel(std::move(model));
  if (moon->getUseTrail())
    this->addTrail(moon->generateTrail());

  Moon *ptr = moon.get();

  centralBody->addMoon(std::move(moon));

  return ptr;
}

AsteroidSystem *Scene::createAsteroidSystem(Object *centralBody, unsigned amount, double innerEdge, double outerEdge)
{
  std::unique_ptr<AsteroidSystem> system = std::make_unique<AsteroidSystem>(centralBody, amount,
                                                                            innerEdge, outerEdge,
                                                                            this->resourceManager.GetMaterial(Res::ASTEROID_MATERIAL), this->threadPool);
  AsteroidSystem *ptr = system.get();
  this->asteroidSystems.push_back(std::move(system));
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
  createPlanet(Res::MARS, Res::MARS_MATERIAL, marsMu, marsRadius, sunPtr, marsElements);
  createAsteroidSystem(sunPtr, 10000, INNER_ASTEROID_BELT_EDGE, OUTER_ASTEROID_BELT_EDGE);
  createPlanet(Res::JUPITER, Res::JUPITER_MATERIAL, jupiterMu, jupiterRadius, sunPtr, jupiterElements);

  auto pointLight = std::make_unique<PointLight>(
      sunPos,
      glm::vec3(0.05f),
      glm::vec3(1.0f),
      glm::vec3(1.0f),
      25.f,
      1.f,
      0.00009f,
      0.0000032f);

  this->addPointLight(std::move(pointLight));

  // auto dirLight = std::make_unique<DirectionalLight>(
  //     glm::vec3(1.2f, 1.0f, 2.0f),
  //     glm::vec3(0.5f),
  //     glm::vec3(1.0f),
  //     glm::vec3(1.0f), 1.f);
  // this->addDirLight(std::move(dirLight));

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

  this->lightManager = std::make_unique<LightManager>();

  this->initShaderBuffer(&this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);
  if (this->directionalLight)
    this->initShaderBuffer(&this->lightManager->getDirUBO(), sizeof(DirLightGPU), GL_UNIFORM_BUFFER);
  // Multiple-lights(not supported on opengl < 4.2)
  // this->initShaderBuffer(&this->lightManager->getPointSSBO(), sizeof(PointLightGPU) * this->pointLights.size(), GL_SHADER_STORAGE_BUFFER);
  if (this->pointLights[0])
    this->initShaderBuffer(&this->lightManager->getPointUBO(), sizeof(PointLightGPU), GL_UNIFORM_BUFFER);
}

void Scene::processKeyboard(CameraMovement direction, float deltaTime)
{
  if (!this->activeCamera)
    throw std::runtime_error("ERROR:SCENE:NO_ACTIVE_CAMERA");

  this->activeCamera->processKeyboard(direction, deltaTime);
}

void Scene::processMouseMovement(const float &xpos, const float &ypos)
{
  if (!this->activeCamera)
    throw std::runtime_error("ERROR:SCENE:NO_ACTIVE_CAMERA");

  this->activeCamera->processMouseMovement(xpos, ypos);
}

void Scene::processMouseScroll(float yoffset)
{
  if (!this->activeCamera)
    throw std::runtime_error("ERROR:SCENE:NO_ACTIVE_CAMERA");

  this->activeCamera->processMouseScroll(yoffset);
}

void Scene::updateUBO(float aspectRatio)
{
  if (!this->activeCamera)
    throw std::runtime_error("ERROR:SCENE:NO_ACTIVE_CAMERA");

  CameraGPU camUBO{};
  camUBO.ProjectionMatrix = this->activeCamera->getProjectionMatrix(aspectRatio);
  camUBO.ViewMatrix = this->activeCamera->getViewMatrix();
  camUBO.camPosition = glm::vec4(this->activeCamera->getPosition(), 1.0);

  glBindBuffer(GL_UNIFORM_BUFFER, this->cameraUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &camUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  if (this->directionalLight)
    this->lightManager->updateDirUBO(*this->directionalLight.get());
  else
    this->lightManager->maskDirUBO();
  if (this->pointLights[0])
    this->lightManager->updatePointUBO(*this->pointLights[0].get());
  else
    this->lightManager->maskPointUBO();
}
void Scene::bindCameraUBO(GLuint programID)
{
  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "Camera");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, CAMERA_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING, cameraUBO);
}

void Scene::update(float dt)
{
  dt *= TIME_SCALE;
  for (size_t i = 0; i < objects.size(); ++i)
  {
    for (size_t j = i + 1; j < objects.size(); ++j)
    {
      objects[i]->applyGravitation(*objects[j]);
      objects[j]->applyGravitation(*objects[i]);
    }
  }
  for (auto &object : this->objects)
  {
    object->update(dt);
  }

  for (auto &asteroidSystem : this->asteroidSystems)
  {
    asteroidSystem->update(dt);
    for (size_t i = 0; i < objects.size(); ++i)
    {
      asteroidSystem->applyObjectGravitation(objects[i].get());
    }
  }
}

void Scene::renderSkybox(Shader *skyboxShader, float aspectRatio)
{
  skyboxShader->use();
  GLuint &skyboxID = skyboxShader->getId();
  this->bindCameraUBO(skyboxID);

  glCullFace(GL_FRONT);

  this->skybox->render(skyboxShader);

  glCullFace(GL_BACK);

  skyboxShader->unuse();
}

void Scene::render(Shader *shader, int framebufferWidth, int framebufferHeight, float dt, Shader *skyboxShader, Shader *asteroidShader, Shader *trailShader)
{
  if (!activeCamera)
    return;

  float aspect = 1.0f;
  if (framebufferHeight > 0)
    aspect = static_cast<float>(framebufferWidth) / framebufferHeight;
  this->update(dt);
  this->updateUBO(aspect);

  trailShader->use();
  GLuint &trailID = trailShader->getId();
  this->bindCameraUBO(trailID);

  for (auto &trail : this->trails)
  {
    trail->render();
  }
  trailShader->unuse();

  shader->use();
  GLuint &coreID = shader->getId();
  this->bindCameraUBO(coreID);

  this->lightManager->bindDirLight(coreID);

  this->lightManager->bindPointLightUBO(coreID);

  // Render all objects
  for (auto &object : this->objects)
  {
    object->render(shader);
  }

  shader->unuse();

  asteroidShader->use();
  GLuint &asteroidID = asteroidShader->getId();
  this->bindCameraUBO(asteroidID);

  this->lightManager->bindDirLight(asteroidID);
  this->lightManager->bindPointLightUBO(asteroidID);

  for (auto &asteroidSystem : asteroidSystems)
  {
    asteroidSystem->render(asteroidShader);
  }

  asteroidShader->unuse();

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
void Scene::addTrail(std::unique_ptr<Trail> trail)
{
  this->trails.push_back(std::move(trail));
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
    throw std::runtime_error("ERROR:SCENE:NO_ACTIVE_CAMERA");
  return *this->activeCamera;
}

const glm::vec3 Scene::getActiveCameraPosition() const
{
  if (!this->activeCamera)
    throw std::runtime_error("ERROR:SCENE:NO_ACTIVE_CAMERA");
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