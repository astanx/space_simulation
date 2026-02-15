#include "render/renderer.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "graphics/bindings/ubo.h"
#include "graphics/bindings/texture.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedCullFace.h"
#include "graphics/state/scopedViewport.h"

#include "graphics/primitives/quad.h"

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
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &camUBO));
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
    glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::Camera);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::Camera, this->cameraUBO);
}

void Renderer::initShaderBuffer(GLuint *ubo, unsigned long size, GLenum bufferType)
{
  glGenBuffers(1, ubo);
  glBindBuffer(bufferType, *ubo);
  GL_CALL(glBufferData(
      bufferType,
      size,
      nullptr,
      GL_DYNAMIC_DRAW));
}

void Renderer::initHDR()
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  glGenFramebuffers(1, &this->hdrFBO);

  this->hdrColorBufferTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

  // create depth buffer (renderbuffer)
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  // attach buffers
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBufferTexture->getId(), 0));
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  std::unique_ptr<Quad> obj = std::make_unique<Quad>();

  this->fullscreenQuad = std::make_unique<Mesh>(std::move(obj), VertexLayout::PositionTexcoord);
}

void Renderer::renderAsteroidSystems(Scene &scene)
{
  Shader &asteroidShader = this->resourceManager.GetShader(Res::ASTEROID_SHADER);

  asteroidShader.use();
  GLuint &asteroidID = asteroidShader.getId();
  this->bindCameraUBO(asteroidID);

  this->lightManager->bindDirLight(asteroidID);
  this->lightManager->bindPointLightUBO(asteroidID);

  this->shadowManager->bindPointShadow(asteroidShader);
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

  this->shadowManager->bindPointShadow(coreShader);
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

  ScopedCullFace cullFace(GL_FRONT);
  ScopedDepthMask depthMask(GL_FALSE);

  skybox.render(skyboxShader);

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

  ScopedViewport viewport(0, 0, shadowRes, shadowRes);

  this->shadowManager->bindDirShadowFBO();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  Shader &dirShadowShader = this->resourceManager.GetShader(Res::DIRECTIONAL_SHADOW_SHADER);

  dirShadowShader.use();

  this->renderShadowMap(scene, dirShadowShader);

  this->shadowManager->unbindPointShadowFBO();
  dirShadowShader.unuse();
}

void Renderer::renderPointShadow(Scene &scene)
{
  const std::vector<PointLight *> &pointLights = scene.getPointLights();

  ScopedViewport viewport(0, 0, shadowRes, shadowRes);

  if (pointLights.empty())
    return;

  Shader &pointShadowShader = this->resourceManager.GetShader(Res::POINT_SHADOW_SHADER);

  this->shadowManager->bindPointShadowFBO();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  pointShadowShader.use();

  this->shadowManager->bindPointShadowUBO(pointShadowShader.getId());

  this->renderShadowMap(scene, pointShadowShader);

  this->shadowManager->unbindPointShadowFBO();

  pointShadowShader.unuse();
}

void Renderer::renderFullscreenQuad()
{
  ScopedDepthMask depthMask(false);
  ScopedDepthTest depthTest(false);

  Shader &hdrShader = this->resourceManager.GetShader(Res::HDR_SHADER);

  hdrShader.use();

  hdrShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  hdrShader.set1i(TextureBindingPoints::BloomBlur, "bloomBlur");
  hdrShader.set1f(0.3f, "exposure");

  this->hdrColorBufferTexture->bind(TextureBindingPoints::HDRColorBuffer);
  this->finalBloomTexture->bind(TextureBindingPoints::BloomBlur);

  this->fullscreenQuad->render();

  hdrShader.unuse();
}

void Renderer::bindHDRFBO()
{
  glBindFramebuffer(GL_FRAMEBUFFER, this->hdrFBO);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::unbindHDRFBO()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::initBloom()
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  glGenFramebuffers(2, this->pingpongFBOs.data());

  for (unsigned int i = 0; i < 2; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFBOs[i]);

    this->pingpongBuffers[i] = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    GL_CALL(glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pingpongBuffers[i]->getId(), 0));
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderBloom()
{
  // todo - framebuffer class, glDisable, etc. in one class, emissive
  glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs[0]);
  glClear(GL_COLOR_BUFFER_BIT);

  Shader &bloomShader = this->resourceManager.GetShader(Res::BLOOM_SHADER);
  bloomShader.use();

  bloomShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  bloomShader.set1f(4.f, "threshold");

  this->fullscreenQuad->render();

  bloomShader.unuse();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::blurBloom()
{
  bool horizontal = true, first_iteration = true;
  int amount = 10;

  Shader &blurShader = this->resourceManager.GetShader(Res::BLUR_SHADER);

  blurShader.use();
  for (unsigned int i = 0; i < amount; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs[horizontal]);

    blurShader.set1i(horizontal, "horizontal");

    if (first_iteration)
      this->pingpongBuffers[0]->bind(TextureBindingPoints::BloomBlur);
    else
      this->pingpongBuffers[!horizontal]->bind(TextureBindingPoints::BloomBlur);

    blurShader.set1i(TextureBindingPoints::BloomBlur, "image");

    this->fullscreenQuad->render();

    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
  }

  this->finalBloomTexture = pingpongBuffers[horizontal].get();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  blurShader.unuse();
}

// Constructor
Renderer::Renderer(ResourceManager &resourceManager) : resourceManager(resourceManager) {}

// Public functions
void Renderer::init(Scene &scene)
{
  this->lightManager = std::make_unique<LightManager>(scene);
  this->shadowManager = std::make_unique<ShadowManager>(scene);

  this->initShaderBuffer(&this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);
  this->initHDR();
  this->initBloom();

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

  {
    ScopedBlending blendingDisabled(false);

    this->bindHDRFBO();

    this->renderTrails(scene);
    this->renderObjects(scene);
    this->renderAsteroidSystems(scene);

    this->renderSkybox(scene);

    this->unbindHDRFBO();
  }

  this->renderBloom();
  this->blurBloom();

  this->renderFullscreenQuad();
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