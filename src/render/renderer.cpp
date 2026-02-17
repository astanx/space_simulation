#include "render/renderer.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "graphics/bindings/ubo.h"
#include "graphics/bindings/texture.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedDepthFunc.h"
#include "graphics/state/scopedCullFace.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedShader.h"

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

  this->hdrFBO = std::make_unique<Framebuffer>();

  this->hdrColorBufferTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

  // create depth buffer (renderbuffer)
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  // attach buffers
  {
    ScopedFramebuffer hdr(*this->hdrFBO, GL_FRAMEBUFFER);

    this->hdrFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBufferTexture->getId(), 0);

    this->hdrFBO->attachRenderBuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    this->hdrFBO->checkComplete();
  }

  std::unique_ptr<Quad> obj = std::make_unique<Quad>();

  this->fullscreenQuad = std::make_unique<Mesh>(std::move(obj), VertexLayout::PositionTexcoord);
}

void Renderer::renderAsteroidSystems(Scene &scene)
{
  Shader &asteroidShader = this->resourceManager.GetShader(Res::ASTEROID_SHADER);

  GLuint &asteroidID = asteroidShader.getId();

  ScopedShader asteroid(asteroidID);

  this->bindCameraUBO(asteroidID);

  this->lightManager->bindDirLight(asteroidID);
  this->lightManager->bindPointLightUBO(asteroidID);

  this->shadowManager->bindPointShadow(asteroidShader);
  this->shadowManager->bindPointShadowUBO(asteroidID);

  for (const AsteroidSystem *asteroidSystem : scene.getAsteroidSystems())
  {
    asteroidSystem->render(asteroidShader);
  }
}

void Renderer::renderObjects(Scene &scene)
{
  Shader &coreShader = this->resourceManager.GetShader(Res::CORE_SHADER);

  GLuint &coreID = coreShader.getId();

  ScopedShader core(coreID);

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
}
void Renderer::renderTrails(Scene &scene)
{
  Shader &trailShader = this->resourceManager.GetShader(Res::TRAIL_SHADER);

  GLuint &trailID = trailShader.getId();

  ScopedShader trail(trailID);

  this->bindCameraUBO(trailID);

  for (const Trail *trail : scene.getTrails())
  {
    trail->render();
  }
}

void Renderer::renderSkybox(Scene &scene)
{
  const Skybox &skybox = scene.getActiveSkybox();

  Shader &skyboxShader = this->resourceManager.GetShader(Res::SKYBOX_SHADER);

  GLuint &skyboxID = skyboxShader.getId();

  ScopedShader skyboxSd(skyboxID);

  this->bindCameraUBO(skyboxID);

  ScopedCullFace cullFace(GL_FRONT);
  ScopedDepthMask depthMask(GL_FALSE);

  skybox.render(skyboxShader);
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
  ScopedFramebuffer dirShadowBuff(this->shadowManager->getDirShadow()->getShadowFramebuffer(), GL_FRAMEBUFFER);

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  Shader &dirShadowShader = this->resourceManager.GetShader(Res::DIRECTIONAL_SHADOW_SHADER);

  ScopedShader dirShadowSd(dirShadowShader);

  this->renderShadowMap(scene, dirShadowShader);
}

void Renderer::renderPointShadow(Scene &scene)
{
  const std::vector<PointLight *> &pointLights = scene.getPointLights();

  ScopedViewport viewport(0, 0, shadowRes, shadowRes);
  ScopedFramebuffer pointShadowBuff(this->shadowManager->getPointShadow()->getShadowFramebuffer(), GL_FRAMEBUFFER);

  if (pointLights.empty())
    return;

  Shader &pointShadowShader = this->resourceManager.GetShader(Res::POINT_SHADOW_SHADER);

  GLuint &pointShadowID = pointShadowShader.getId();

  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT);

  ScopedShader pointShadowSd(pointShadowID);

  this->shadowManager->bindPointShadowUBO(pointShadowID);

  this->renderShadowMap(scene, pointShadowShader);
}

void Renderer::renderFullscreenQuad()
{
  ScopedDepthMask depthMask(false);
  ScopedDepthTest depthTest(false);

  Shader &hdrShader = this->resourceManager.GetShader(Res::HDR_SHADER);

  ScopedShader hdr(hdrShader);

  ScopedTexture hdrColor(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);
  ScopedTexture bloom(*this->finalBloomTexture, TextureBindingPoints::BloomBlur);

  hdrShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  hdrShader.set1i(TextureBindingPoints::BloomBlur, "bloomBlur");
  hdrShader.set1f(0.3f, "exposure");

  this->fullscreenQuad->render();
}

void Renderer::initBloom()
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  for (unsigned int i = 0; i < 2; i++)
  {
    this->pingpongFBOs[i] = std::make_unique<Framebuffer>();

    ScopedFramebuffer pingpong(*this->pingpongFBOs[i], GL_FRAMEBUFFER, true);

    this->pingpongBuffers[i] = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    this->pingpongFBOs[i]->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pingpongBuffers[i]->getId(), 0);

    this->pingpongFBOs[i]->checkComplete();
  }

  this->finalBloomTexture = this->pingpongBuffers[0].get();
}

void Renderer::renderBloom()
{
  // todo - emissive & sun hdr does not work, bloom turn on/off
  ScopedFramebuffer pingpong(*this->pingpongFBOs[0], GL_FRAMEBUFFER);
  glClear(GL_COLOR_BUFFER_BIT);

  Shader &bloomShader = this->resourceManager.GetShader(Res::BLOOM_SHADER);

  ScopedShader bloom(bloomShader);

  ScopedTexture hdrColorBuffer(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);

  bloomShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  bloomShader.set1f(4.f, "threshold");

  this->fullscreenQuad->render();
}

void Renderer::blurBloom()
{
  bool horizontal = true, first_iteration = true;
  int amount = 10;

  Shader &blurShader = this->resourceManager.GetShader(Res::BLUR_SHADER);

  ScopedShader blur(blurShader);

  for (unsigned int i = 0; i < amount; i++)
  {
    ScopedFramebuffer pingpong(*this->pingpongFBOs[horizontal], GL_FRAMEBUFFER, true);

    std::unique_ptr<ScopedTexture> pingpongBuffScope;

    if (first_iteration)
      pingpongBuffScope = std::make_unique<ScopedTexture>(*this->pingpongBuffers[0], TextureBindingPoints::BloomBlur);
    else
      pingpongBuffScope = std::make_unique<ScopedTexture>(*this->pingpongBuffers[!horizontal], TextureBindingPoints::BloomBlur);

    blurShader.set1i(horizontal, "horizontal");
    blurShader.set1i(TextureBindingPoints::BloomBlur, "image");

    this->fullscreenQuad->render();

    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
  }

  this->finalBloomTexture = pingpongBuffers[horizontal].get();
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

    ScopedFramebuffer hdr(*this->hdrFBO, GL_FRAMEBUFFER);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->renderObjects(scene);
    this->renderAsteroidSystems(scene);
  }

  this->renderBloom();
  this->blurBloom();

  this->renderFullscreenQuad();

  {
    ScopedFramebuffer hdrRead(*this->hdrFBO, GL_READ_FRAMEBUFFER);
    ScopedFramebuffer draw(0, GL_DRAW_FRAMEBUFFER);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glBlitFramebuffer(0, 0, viewport[2], viewport[3], 0, 0, viewport[2], viewport[3], GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  }

  this->renderSkybox(scene);
  this->renderTrails(scene);
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