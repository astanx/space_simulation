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

#include "graphics/buffers/renderBuffer.h"

#include "graphics/primitives/quad.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "physics/star.h"
#include "physics/planet.h"

#include <iostream>

// Private functions
void Renderer::updateUBO(Scene &scene)
{
  const Camera &activeCamera = scene.getActiveCamera();

  if (!glIsBuffer(this->cameraUBO))
    Logger::logWarning("Renderer", "No camera UBO to update");
  else
  {
    CameraGPU camUBO{};
    camUBO.ProjectionMatrix = activeCamera.getProjectionMatrix(this->ctx->aspect);
    camUBO.ViewMatrix = activeCamera.getViewMatrix();
    // camUBO.camPosition = glm::vec4(activeCamera.getPosition(), 1.0);
    // camUBO.camPosition = glm::vec4(glm::vec3(0.f), 1.0);
    camUBO.camPosition = glm::vec4(0.0);

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
      this->lightManager->updatePointUBO(pointLights[i]);
  }
  else
    this->lightManager->maskPointUBO();

  this->shadowManager->updatePointUBO();
}
void Renderer::initShaderUBOBindings()
{
  std::vector<Shader *> shaders = this->resourceManager.GetAllShaders();

  for (Shader *shader : shaders)
  {
    if (!shader)
      continue;

    GLuint programID = shader->getId();

    GLuint blockIndex = glGetUniformBlockIndex(programID, "Camera");
    if (blockIndex != GL_INVALID_INDEX)
      glUniformBlockBinding(programID, blockIndex, UBOBindingPoints::Camera);

    this->lightManager->initDirLightUBOBinding(programID);
    this->lightManager->initPointLightUBOBinding(programID);

    this->shadowManager->initDirShadowUBOBinding(programID);
    this->shadowManager->initPointShadowUBOBinding(programID);
  }
}

void Renderer::bindUBOs()
{
  glBindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::Camera, this->cameraUBO);

  this->lightManager->bindDirLightUBO();
  this->lightManager->bindPointLightUBO();

  this->shadowManager->bindDirShadowUBO();
  this->shadowManager->bindPointShadowUBO();
}

void Renderer::bindDummyReflector(Shader &shader)
{
  glActiveTexture(GL_TEXTURE0 + TextureBindingPoints::EnvironmentMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, TextureBindingPoints::EnvironmentMap);
  shader.set1i(TextureBindingPoints::EnvironmentMap, "reflectorRadianceCubemap");
  shader.set1i(0, "useReflectorRadiance");
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

void Renderer::renderAsteroidSystems(Scene &scene, Frustum *frustum)
{
  Shader &asteroidShader = this->resourceManager.GetShader(Res::ASTEROID_SHADER);

  GLuint &asteroidID = asteroidShader.getId();

  const Skybox &skybox = scene.getActiveSkybox();

  ScopedShader asteroid(asteroidID);

  this->shadowManager->bindPointShadow(asteroidShader);
  this->shadowManager->bindPointShadowDepth(asteroidShader);

  skybox.bindIrradianceMap(asteroidShader);

  for (const AsteroidSystem *asteroidSystem : scene.getAsteroidSystems())
    asteroidSystem->render(asteroidShader, frustum);

  skybox.unbindIrradianceMap();
}

void Renderer::renderObjects(Scene &scene, Frustum *frustum)
{
  Shader &coreShader = this->resourceManager.GetShader(Res::CORE_SHADER);

  GLuint &coreID = coreShader.getId();

  const Skybox &skybox = scene.getActiveSkybox();

  ScopedShader core(coreID);

  this->shadowManager->bindPointShadow(coreShader);
  this->shadowManager->bindPointShadowDepth(coreShader);

  skybox.bindIrradianceMap(coreShader);

  this->bindDummyReflector(coreShader);

  // Render all objects
  for (const Renderable *object : scene.getRenderable())
    object->render(coreShader, frustum);

  skybox.unbindIrradianceMap();
}
void Renderer::renderTrails(Scene &scene)
{
  Shader &trailShader = this->resourceManager.GetShader(Res::TRAIL_SHADER);

  GLuint &trailID = trailShader.getId();

  ScopedShader trail(trailID);

  for (const Trail *trail : scene.getTrails())
    trail->render();
}

void Renderer::renderSkybox(Scene &scene)
{
  const Skybox &skybox = scene.getActiveSkybox();

  Shader &skyboxShader = this->resourceManager.GetShader(Res::SKYBOX_SHADER);

  GLuint &skyboxID = skyboxShader.getId();

  ScopedShader skyboxSd(skyboxID);

  skyboxShader.set1f(this->ctx->exposure, "exposure");

  ScopedCullFace cullFace(GL_FRONT);
  ScopedDepthMask depthMask(GL_FALSE);

  skybox.render(skyboxShader);
}

void Renderer::renderShadowMap(Scene &scene, Shader &shader)
{
  for (const Renderable *object : scene.getRenderable())
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

  glClearColor(exp(40.0f), 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ScopedShader pointShadowSd(pointShadowID);
  // ScopedCullFace cullFace(GL_FRONT);

  this->renderShadowMap(scene, pointShadowShader);

  this->blur.blur(this->shadowManager->getPointShadow()->getShadowMapTexture(), 16, true);
}

void Renderer::renderToFramebuffer(Scene &scene, const Framebuffer &framebuffer, bool useFramebuffer)
{
  ScopedBlending blendingDisabled(false);
  std::optional<ScopedFramebuffer> framebufferScope;

  if (useFramebuffer)
    framebufferScope.emplace(framebuffer, GL_FRAMEBUFFER);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Frustum frustum = scene.getActiveCamera().getFrustum(this->ctx->aspect);

  this->renderObjects(scene, &frustum);
  this->renderAsteroidSystems(scene, &frustum);
  this->renderSkybox(scene);
}

void Renderer::blitDepthToDefault(const Framebuffer &framebuffer)
{
  ScopedFramebuffer hdrRead(framebuffer, GL_READ_FRAMEBUFFER);
  ScopedFramebuffer draw(0, GL_DRAW_FRAMEBUFFER);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glBlitFramebuffer(0, 0, viewport[2], viewport[3], 0, 0, viewport[2], viewport[3], GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void Renderer::renderMoonsRadiance(Scene &scene)
{
  Shader &moonsRadianceShader = this->resourceManager.GetShader(Res::REFLECTION_SHADER);

  ScopedShader moon(moonsRadianceShader);

  moonsRadianceShader.set1f(scene.getSun().getLuminosity(), "lightLuminocity");

  for (const Planet *planet : scene.getPlanetarObjects())
    planet->renderMoonsRadiance(moonsRadianceShader, scene.getActiveCamera());
}

// Constructor
Renderer::Renderer(ResourceManager &resourceManager) : resourceManager(resourceManager), postProcess(resourceManager), blur(resourceManager) {}

// Public functions
void Renderer::init(Scene &scene)
{
  this->lightManager = std::make_unique<LightManager>(scene);
  this->shadowManager = std::make_unique<ShadowManager>(scene);

  this->initShaderBuffer(&this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);
  this->blur.init(37, 6.2f, true, this->shadowRes);

  const DirectionalLight *directionalLight = scene.getDirLight();
  const std::vector<PointLight *> &pointLights = scene.getPointLights();

  if (directionalLight)
    this->shadowManager->addDirShadow(std::make_unique<DirectionalShadow>(this->shadowRes, this->shadowRes));

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

  this->ctx = scene.getFrameContext();

  this->textRenderer.init();
  this->postProcess.init(ctx);

  this->initShaderUBOBindings();
}

void Renderer::render(Scene &scene, bool useBloom, bool useHDR)
{
  this->bindUBOs();

  this->renderPointShadow(scene);
  this->renderDirectionalShadow(scene);

  this->renderMoonsRadiance(scene);

  const Framebuffer &hdrFramebuffer = this->postProcess.getHDRFramebuffer();

  this->renderToFramebuffer(scene, hdrFramebuffer, useHDR);

  if (useHDR)
  {
    this->postProcess.process(useBloom);

    this->blitDepthToDefault(hdrFramebuffer);
  }

  this->renderTrails(scene);
}

void Renderer::update(Scene &scene, double dt, bool paused)
{
  scene.update(dt, paused);

  this->updateUBO(scene);
}

void Renderer::renderText(const std::string &text, float x, float y, float scale, glm::vec3 color)
{
  Shader &textShader = this->resourceManager.GetShader(Res::TEXT_SHADER);
  this->textRenderer.render(textShader, text, x, y, scale, color);
}