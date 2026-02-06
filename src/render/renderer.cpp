#include "render/renderer.h"
#include "scene/scene.h"
#include "resources/resourceManager.h"
#include "physics/star.h"

#include <iostream>

// Private functions
void Renderer::updateUBO(Scene &scene, float aspectRatio)
{
  Camera &activeCamera = scene.getActiveCamera();

  CameraGPU camUBO{};
  camUBO.ProjectionMatrix = activeCamera.getProjectionMatrix(aspectRatio);
  camUBO.ViewMatrix = activeCamera.getViewMatrix();
  camUBO.camPosition = glm::vec4(activeCamera.getPosition(), 1.0);

  glBindBuffer(GL_UNIFORM_BUFFER, this->cameraUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &camUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  const std::unique_ptr<DirectionalLight> &directionalLight = scene.getDirLight();
  if (directionalLight)
    this->lightManager->updateDirUBO(*directionalLight.get());
  else
    this->lightManager->maskDirUBO();

  this->shadowManager->updateDirUBO();

  const std::vector<std::unique_ptr<PointLight>> &pointLights = scene.getPointLights();
  if (!pointLights.empty())
  {
    this->shadowManager->updatePointShadowLightPosition(scene.getSun().getRenderPosition());
    for (size_t i = 0; i < pointLights.size(); i++)
    {
      this->lightManager->updatePointUBO(*pointLights[i].get());
    }
  }
  else
    this->lightManager->maskPointUBO();

  this->shadowManager->updatePointUBO();
}

void Renderer::bindCameraUBO(GLuint programID)
{
  GLuint blockIndex =
      glGetUniformBlockIndex(programID, "Camera");

  if (blockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programID, blockIndex, CAMERA_BINDING);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING, cameraUBO);
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
  this->asteroidShader->use();
  GLuint &asteroidID = asteroidShader->getId();
  this->bindCameraUBO(asteroidID);

  this->lightManager->bindDirLight(asteroidID);
  this->lightManager->bindPointLightUBO(asteroidID);

  this->shadowManager->bindPointShadow(*asteroidShader, 5);
  this->shadowManager->bindPointShadowUBO(asteroidID);

  for (auto &asteroidSystem : scene.getAsteroidSystems())
  {
    asteroidSystem->render(asteroidShader);
  }

  this->asteroidShader->unuse();
}

void Renderer::renderObjects(Scene &scene)
{
  this->coreShader->use();

  GLuint &coreID = this->coreShader->getId();
  this->bindCameraUBO(coreID);

  this->lightManager->bindDirLight(coreID);
  this->lightManager->bindPointLightUBO(coreID);

  this->shadowManager->bindPointShadow(*this->coreShader, 5);
  this->shadowManager->bindPointShadowUBO(coreID);

  // Render all objects
  for (auto &object : scene.getObjects())
  {
    object->render(this->coreShader);
  }

  this->coreShader->unuse();
}
void Renderer::renderTrails(Scene &scene)
{
  this->trailShader->use();
  GLuint &trailID = trailShader->getId();
  this->bindCameraUBO(trailID);

  for (auto &trail : scene.getTrails())
  {
    trail->render();
  }
  this->trailShader->unuse();
}

void Renderer::renderSkybox(Scene &scene)
{
  Skybox &skybox = scene.getActiveSkybox();

  this->skyboxShader->use();
  GLuint &skyboxID = skyboxShader->getId();
  this->bindCameraUBO(skyboxID);

  glCullFace(GL_FRONT);

  skybox.render(this->skyboxShader);

  glCullFace(GL_BACK);

  this->skyboxShader->unuse();
}

void Renderer::renderShadowMap(Scene &scene, Shader &shader) // +
{
  for (auto &object : scene.getObjects())
  {
    if (dynamic_cast<Star *>(object.get()))
      continue;

    object->render(&shader);
  }
}

void Renderer::renderDirectionalShadow(Scene &scene) // +
{
  if (!scene.getDirLight())
    return;

  GLint prevViewport[4];
  glGetIntegerv(GL_VIEWPORT, prevViewport);

  glViewport(0, 0, shadowRes, shadowRes);

  this->shadowManager->bindDirShadowFBO();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  this->dirShadowShader->use();

  this->renderShadowMap(scene, *dirShadowShader);

  this->shadowManager->unbindPointShadowFBO();
  this->dirShadowShader->unuse();

  glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
}

void Renderer::renderPointShadow(Scene &scene) // +
{
  const std::vector<std::unique_ptr<PointLight>> &pointLights = scene.getPointLights();

  GLint prevViewport[4];
  glGetIntegerv(GL_VIEWPORT, prevViewport);

  if (pointLights.empty())
    return;

  glViewport(0, 0, shadowRes, shadowRes);

  this->shadowManager->bindPointShadowFBO();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  this->pointShadowShader->use();

  this->shadowManager->bindPointShadowUBO(this->pointShadowShader->getId());

  this->renderShadowMap(scene, *this->pointShadowShader);

  this->shadowManager->unbindPointShadowFBO();

  this->pointShadowShader->unuse();

  glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
}

// Constructor
Renderer::Renderer(ResourceManager &resourceManager) : resourceManager(resourceManager) {}

// Public functions
void Renderer::init(Scene &scene)
{
  this->coreShader = this->resourceManager.GetShader(Res::CORE_SHADER);
  this->asteroidShader = this->resourceManager.GetShader(Res::ASTEROID_SHADER);
  this->trailShader = this->resourceManager.GetShader(Res::TRAIL_SHADER);
  this->pointShadowShader = this->resourceManager.GetShader(Res::POINT_SHADOW_SHADER);
  this->textShader = this->resourceManager.GetShader(Res::TEXT_SHADER);
  this->skyboxShader = this->resourceManager.GetShader(Res::SKYBOX_SHADER);

  this->lightManager = std::make_unique<LightManager>();
  this->shadowManager = std::make_unique<ShadowManager>();

  this->initShaderBuffer(&this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);

  const std::unique_ptr<DirectionalLight> &directionalLight = scene.getDirLight();
  const std::vector<std::unique_ptr<PointLight>> &pointLights = scene.getPointLights();

  if (directionalLight)
  {
    this->initShaderBuffer(&this->lightManager->getDirUBO(), sizeof(DirLightGPU), GL_UNIFORM_BUFFER);
    this->initShaderBuffer(&this->shadowManager->getDirUBO(), sizeof(DirShadowGPU), GL_UNIFORM_BUFFER);
    this->shadowManager->addDirShadow(std::make_unique<DirectionalShadow>(this->shadowRes, this->shadowRes));
  }
  // Multiple-lights(not supported on opengl < 4.2)
  // this->initShaderBuffer(&this->lightManager->getPointSSBO(), sizeof(PointLightGPU) * this->pointLights.size(), GL_SHADER_STORAGE_BUFFER);
  if (!pointLights.empty())
  {
    Camera &activeCamera = scene.getActiveCamera();

    this->initShaderBuffer(&this->lightManager->getPointUBO(), sizeof(PointLightGPU), GL_UNIFORM_BUFFER);
    this->initShaderBuffer(&this->shadowManager->getPointUBO(), sizeof(PointShadowGPU), GL_UNIFORM_BUFFER);

    for (auto &light : pointLights)
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

void Renderer::update(Scene &scene, float dt, bool paused)
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
  this->textRenderer.render(*this->textShader, text, x, y, scale, color);
}