#include "render/renderer.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "physics/star.h"

#include <iostream>

// Private functions
void Renderer::updateUBO(Scene &scene, float aspectRatio)
{
  const Camera &activeCamera = scene.getActiveCamera();

  if (!glIsBuffer(this->cameraUBO))
    Logger::logWarning("Renderer", "No camera UBO to update");
  else
  {
    CameraGPU camUBO{};
    camUBO.ProjectionMatrix = activeCamera.getProjectionMatrix(aspectRatio);
    camUBO.ViewMatrix = activeCamera.getViewMatrix();
    camUBO.camPosition = glm::vec4(activeCamera.getPosition(), 1.0);

    glBindBuffer(GL_UNIFORM_BUFFER, this->cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &camUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  const DirectionalLight *directionalLight = scene.getDirLight();
  if (directionalLight)
    this->lightManager->updateDirUBO(directionalLight);
  else
    this->lightManager->maskDirUBO();

  this->shadowManager->updateDirUBO();

  const std::vector<PointLight *> &pointLights = scene.getPointLights();
  if (!pointLights.empty())
  {
    this->shadowManager->updatePointShadowLightPosition(scene.getSun().getRenderPosition());
    for (size_t i = 0; i < pointLights.size(); i++)
    {
      this->lightManager->updatePointUBO(pointLights[i]);
    }
  }
  else
    this->lightManager->maskPointUBO();

  this->shadowManager->updatePointUBO();
}

void Renderer::bindCameraUBO(GLuint programID)
{
  if (!glIsBuffer(this->cameraUBO))
    Logger::logWarning("Renderer", "No camera UBO to bind");

  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "Camera");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, CAMERA_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING, this->cameraUBO);
}

void Renderer::initShaderBuffer(GLuint *ubo, unsigned long size, GLenum bufferType)
{
  glGenBuffers(1, ubo);
  glBindBuffer(bufferType, *ubo);
  glBufferData(
      bufferType,
      size,
      nullptr,
      GL_DYNAMIC_DRAW);
}

void Renderer::renderAsteroidSystems(Scene &scene)
{
  Shader &asteroidShader = this->resourceManager.GetShader(Res::ASTEROID_SHADER);

  asteroidShader.use();
  GLuint &asteroidID = asteroidShader.getId();
  this->bindCameraUBO(asteroidID);

  this->lightManager->bindDirLight(asteroidID);
  this->lightManager->bindPointLightUBO(asteroidID);

  this->shadowManager->bindPointShadow(asteroidShader, 5);
  this->shadowManager->bindPointShadowUBO(asteroidID);

  for (const AsteroidSystem *asteroidSystem : scene.getAsteroidSystems())
  {
    asteroidSystem->render(asteroidShader);
  }

  asteroidShader.unuse();
}

void Renderer::renderObjects(Scene &scene)
{
  Shader &coreShader = this->resourceManager.GetShader(Res::CORE_SHADER);

  coreShader.use();

  GLuint &coreID = coreShader.getId();
  this->bindCameraUBO(coreID);

  this->lightManager->bindDirLight(coreID);
  this->lightManager->bindPointLightUBO(coreID);

  this->shadowManager->bindPointShadow(coreShader, 5);
  this->shadowManager->bindPointShadowUBO(coreID);

  // Render all objects
  for (const Object *object : scene.getObjects())
  {
    object->render(coreShader);
  }

  coreShader.unuse();
}
void Renderer::renderTrails(Scene &scene)
{
  Shader &trailShader = this->resourceManager.GetShader(Res::TRAIL_SHADER);

  trailShader.use();
  GLuint &trailID = trailShader.getId();
  this->bindCameraUBO(trailID);

  for (const Trail *trail : scene.getTrails())
  {
    trail->render();
  }
  trailShader.unuse();
}

void Renderer::renderSkybox(Scene &scene)
{
  const Skybox &skybox = scene.getActiveSkybox();

  Shader &skyboxShader = this->resourceManager.GetShader(Res::SKYBOX_SHADER);

  skyboxShader.use();
  GLuint &skyboxID = skyboxShader.getId();
  this->bindCameraUBO(skyboxID);

  glCullFace(GL_FRONT);

  skybox.render(skyboxShader);

  glCullFace(GL_BACK);

  skyboxShader.unuse();
}

void Renderer::renderShadowMap(Scene &scene, Shader &shader)
{
  for (const Object *object : scene.getObjects())
  {
    if (dynamic_cast<const Star *>(object))
      continue;

    object->render(shader);
  }
}

void Renderer::renderDirectionalShadow(Scene &scene)
{
  if (!scene.getDirLight())
    return;

  GLint prevViewport[4];
  glGetIntegerv(GL_VIEWPORT, prevViewport);

  glViewport(0, 0, shadowRes, shadowRes);

  this->shadowManager->bindDirShadowFBO();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  Shader &dirShadowShader = this->resourceManager.GetShader(Res::DIRECTIONAL_SHADOW_SHADER);

  dirShadowShader.use();

  this->renderShadowMap(scene, dirShadowShader);

  this->shadowManager->unbindPointShadowFBO();
  dirShadowShader.unuse();

  glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
}

void Renderer::renderPointShadow(Scene &scene) // +
{
  const std::vector<PointLight *> &pointLights = scene.getPointLights();

  GLint prevViewport[4];
  glGetIntegerv(GL_VIEWPORT, prevViewport);

  if (pointLights.empty())
    return;

  glViewport(0, 0, shadowRes, shadowRes);

  Shader &pointShadowShader = this->resourceManager.GetShader(Res::POINT_SHADOW_SHADER);

  this->shadowManager->bindPointShadowFBO();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  pointShadowShader.use();

  this->shadowManager->bindPointShadowUBO(pointShadowShader.getId());

  this->renderShadowMap(scene, pointShadowShader);

  this->shadowManager->unbindPointShadowFBO();

  pointShadowShader.unuse();

  glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
}

// Constructor
Renderer::Renderer(ResourceManager &resourceManager) : resourceManager(resourceManager) {}

// Public functions
void Renderer::init(Scene &scene)
{
  this->lightManager = std::make_unique<LightManager>(scene);
  this->shadowManager = std::make_unique<ShadowManager>(scene);

  this->initShaderBuffer(&this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);

  const DirectionalLight *directionalLight = scene.getDirLight();
  const std::vector<PointLight *> &pointLights = scene.getPointLights();

  if (directionalLight)
  {
    this->shadowManager->addDirShadow(std::make_unique<DirectionalShadow>(this->shadowRes, this->shadowRes));
  }
  // Multiple-lights(not supported on opengl < 4.2)
  // this->initShaderBuffer(&this->lightManager->getPointSSBO(), sizeof(PointLightGPU) * this->pointLights.size(), GL_SHADER_STORAGE_BUFFER);
  if (!pointLights.empty())
  {
    const Camera &activeCamera = scene.getActiveCamera();

    for (PointLight *light : pointLights)
      this->shadowManager->addPointShadow(std::make_unique<PointShadow>(this->shadowRes, this->shadowRes,
                                                                        light->getPosition(),
                                                                        activeCamera.getNearPlane(),
                                                                        activeCamera.getFarPlane()));
  }

  this->textRenderer.init();
}

void Renderer::render(Scene &scene)
{
  this->renderPointShadow(scene);
  this->renderDirectionalShadow(scene);

  this->renderTrails(scene);
  this->renderObjects(scene);
  this->renderAsteroidSystems(scene);

  this->renderSkybox(scene);
}

void Renderer::update(Scene &scene, double dt, bool paused)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  float aspect = 1.f;
  if (height != 0)
    aspect = width / height;

  if (!paused)
    scene.update(dt);

  this->updateUBO(scene, aspect);
}

void Renderer::renderText(const std::string &text, float x, float y, float scale, glm::vec3 color)
{
  Shader &textShader = this->resourceManager.GetShader(Res::TEXT_SHADER);
  this->textRenderer.render(textShader, text, x, y, scale, color);
}