#include "render/renderer.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "render/renderState.h"

#include "graphics/instanceLayouts.h"

#include "graphics/bindings/ubo.h"
#include "graphics/bindings/texture.h"
#include "graphics/bindings/impostor.h"

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
#include "graphics/primitives/point.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "physics/star.h"
#include "physics/planet.h"
#include "physics/systems/asteroidSystem.h"

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "external/stb_image_resize2.h"

#include <iostream>

// Private functions
void Renderer::updateUBO(Scene &scene, RenderContext &ctx)
{
  const Camera &activeCamera = scene.getActiveCamera();

  if (!glIsBuffer(this->cameraUBO))
    Logger::logWarning("Renderer", "No camera UBO to update");
  else
  {
    CameraGPU camUBO{};
    camUBO.ProjectionMatrix = activeCamera.getProjectionMatrix(ctx.frameCtx.aspect);
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
    this->shadowManager->updatePointShadowLightPosition(scene.getPhysicsWorld().getSun().getRenderPosition());
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

    GLuint cameraBlockIndex = glGetUniformBlockIndex(programID, "Camera");
    if (cameraBlockIndex != GL_INVALID_INDEX)
      glUniformBlockBinding(programID, cameraBlockIndex, UBOBindingPoints::Camera);

    GLuint scaleBlockIndex = glGetUniformBlockIndex(programID, "Scale");
    if (scaleBlockIndex != GL_INVALID_INDEX)
      glUniformBlockBinding(programID, scaleBlockIndex, UBOBindingPoints::Scale);

    this->lightManager->initDirLightUBOBinding(programID);
    this->lightManager->initPointLightUBOBinding(programID);

    this->shadowManager->initDirShadowUBOBinding(programID);
    this->shadowManager->initPointShadowUBOBinding(programID);
  }
}

void Renderer::initLOD(Scene &scene)
{
  this->initImpostor(scene);
  this->initPoint();
}

void Renderer::initImpostor(Scene &scene)
{
  this->impostorMesh = std::make_unique<Mesh>(TypeTag<VertexPositionTexcoord>{}, std::make_unique<Quad>(), VertexLayout::PositionTexcoord);
  this->impostorMesh->setInstanceLayout(InstanceLayout::PositionRadiusTexture);
  this->impostorMesh->setInstanceBuffer(this->impostorInstances.data(), this->impostorInstances.size());

  this->impostorTexture = std::make_unique<Texture>(GL_TEXTURE_2D_ARRAY);

  unsigned int layer = ImpostorTextureBindingPoints::Size;
  for (const ModelSource *object : scene.getModelSources())
  {
    const Texture *texture = object->getMainLayer()->getMaterial()->getTexture();
    if (!texture)
      Logger::logWarning("Renderer", "No texture found for object");

    this->bindLayerToImpostorTexture(*texture, layer);
    layer++;
  }

  for (System *system : scene.getPhysicsWorld().getSystems())
  {
    const Texture *texture = system->getTexture();
    if (!texture)
      Logger::logWarning("Renderer", "No texture found for system");

    this->bindLayerToImpostorTexture(*texture, layer);

    system->setImpostorLayer(layer);
    layer++;
  }
}

void Renderer::initPoint()
{
  this->pointMesh = std::make_unique<Mesh>(TypeTag<VertexEmpty>{}, std::make_unique<Point>(), VertexLayout::Empty, GL_POINTS);
  this->pointMesh->setInstanceLayout(InstanceLayout::PositionRadiusColor);
  this->pointMesh->setInstanceBuffer(this->pointInstances.data(), this->pointInstances.size());
}

void Renderer::bindLayerToImpostorTexture(const Texture &texture, unsigned int layer)
{
  if (!this->impostorTexture)
    Logger::logFatal("Renderer", "No importor texture to bind layer");

  ScopedTexture impostor(*this->impostorTexture);

  int width = 1024;
  int height = 512;

  int srcWidth = texture.getWidth();
  int srcHeight = texture.getHeight();

  if (!this->isImpostorInitialized)
  {
    GL_CALL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, ImpostorTextureBindingPoints::MaxSize));
    this->isImpostorInitialized = true;
  }

  if (layer >= ImpostorTextureBindingPoints::MaxSize)
    Logger::logError("Renderer", "Maximum impostor layer size exceeded");

  this->textureLayers[texture.getId()] = layer;

  std::vector<uint8_t> pixels(srcWidth * srcHeight * 4);

  {
    ScopedTexture text(texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  }

  std::vector<uint8_t> resized(width * height * 4);

  stbir_resize_uint8_linear(pixels.data(), srcWidth, srcHeight, 0, resized.data(), width, height, 0, STBIR_RGBA);

  GL_CALL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, resized.data()));
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

  {
    ScopedShader asteroid(asteroidID);

    this->shadowManager->bindPointShadow(asteroidShader);
    this->shadowManager->bindPointShadowDepth(asteroidShader);

    skybox.bindIrradianceMap(asteroidShader);

    for (const AsteroidSystem *asteroidSystem : scene.getPhysicsWorld().getAsteroidSystems())
      asteroidSystem->render(asteroidShader, frustum);

    skybox.unbindIrradianceMap();
  }
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
  for (const Renderable *object : this->fullInstances)
    object->render(coreShader, frustum);

  skybox.unbindIrradianceMap();
}
void Renderer::renderAtmospheres(Scene &scene, Frustum *frustum)
{
  Shader &atmosphereShader = this->resourceManager.GetShader(Res::ATMOSPHERE_SHADER);

  GLuint &atmosphereID = atmosphereShader.getId();

  ScopedShader atmosphereSh(atmosphereID);

  for (const Planet *planet : scene.getPhysicsWorld().getPlanetarObjects())
    planet->renderAtmosphere(atmosphereShader, frustum);
}
void Renderer::renderTrails(Scene &scene)
{
  Shader &trailShader = this->resourceManager.GetShader(Res::TRAIL_SHADER);

  GLuint &trailID = trailShader.getId();

  ScopedShader trail(trailID);

  for (const Trail *trail : scene.getTrails())
    trail->render();
}

void Renderer::renderSkybox(Scene &scene, RenderContext &ctx)
{
  const Skybox &skybox = scene.getActiveSkybox();

  Shader &skyboxShader = this->resourceManager.GetShader(Res::SKYBOX_SHADER);

  GLuint &skyboxID = skyboxShader.getId();

  ScopedShader skyboxSd(skyboxID);

  skyboxShader.set1f(ctx.settings.useHDR ? ctx.settings.exposure : 1.f, "exposure");

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

  RenderState::clearDepth();

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

  RenderState::clearColor(glm::vec4(exp(40.0f), 0, 0, 0));
  RenderState::clearDepth();

  ScopedShader pointShadowSd(pointShadowID);
  // ScopedCullFace cullFace(GL_FRONT);

  this->renderShadowMap(scene, pointShadowShader);

  this->blur.blur(this->shadowManager->getPointShadow()->getShadowMapTexture(), 16, true);
}

void Renderer::renderImpostor(Scene &scene)
{
  Shader &impostorShader = this->resourceManager.GetShader(Res::IMPOSTOR_SHADER);
  GLuint &impostorID = impostorShader.getId();

  ScopedShader impostor(impostorID);
  ScopedTexture impostorText(*this->impostorTexture, ImpostorTextureBindingPoints::Impostor);

  const Skybox &skybox = scene.getActiveSkybox();
  skybox.bindIrradianceMap(impostorShader);

  impostorShader.set1i(ImpostorTextureBindingPoints::Impostor, "impostors");

  this->impostorMesh->renderInstanced();

  skybox.unbindIrradianceMap();
}

void Renderer::renderPoint()
{
  Shader &pointShader = this->resourceManager.GetShader(Res::POINT_SHADER);
  GLuint &pointID = pointShader.getId();

  ScopedShader point(pointID);

  this->pointMesh->renderInstanced();
}

void Renderer::renderToFramebuffer(Scene &scene, const Framebuffer &framebuffer, RenderContext &ctx)
{
  ScopedBlending blendingDisabled(false);
  std::optional<ScopedFramebuffer> framebufferScope;

  if (ctx.settings.useHDR)
    framebufferScope.emplace(framebuffer, GL_FRAMEBUFFER);

  RenderState::clearColor(ctx.settings.clearColor);
  RenderState::clearDepth();

  Frustum frustum = scene.getActiveCamera().getFrustum(ctx.frameCtx.aspect);

  this->renderObjects(scene, &frustum);
  this->renderImpostor(scene);
  this->renderPoint();
  // temporary off
  //  this->renderAtmospheres(scene, &frustum);
  this->renderAsteroidSystems(scene, &frustum);
  this->renderSkybox(scene, ctx);
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

  moonsRadianceShader.set1f(scene.getPhysicsWorld().getSun().getLuminosity(), "lightLuminocity");

  for (const Planet *planet : scene.getPhysicsWorld().getPlanetarObjects())
    planet->renderMoonsRadiance(moonsRadianceShader, scene.getActiveCamera());
}

void Renderer::beginFrame(RenderContext &ctx)
{
  RenderState::clearColor(ctx.settings.clearColor);
  RenderState::clearDepth();
  glClear(GL_STENCIL_BUFFER_BIT);
}

void Renderer::partitionObject(ModelSource *object, Frustum *frustum, float viewportHeight, float fov, bool force)
{
  float importance = object->getRenderImportance();
  float radius = object->getWorldRadius();
  const glm::dvec3 pos = object->getRenderPosition();
  float scaledMeanRadius = this->lodManager.scaleRadius(pos, radius, fov, viewportHeight, importance);
  object->setRenderRadius(scaledMeanRadius);

  if (!Frustum::shouldBeProcessed(frustum, pos, scaledMeanRadius, force))
    return;

  const Texture *texture = object->getMainLayer()->getMaterial()->getTexture();

  int level = this->lodManager.getLODLevel(pos, radius, fov, viewportHeight, importance);

  switch (level)
  {
  case LOD::Full:
  {
    const Radii &radii = object->getSrcRadii();
    float scaledEquatorianRadius = this->lodManager.scaleRadius(pos, radii.equatorian, fov, viewportHeight, importance);
    float scaledPolarRadius = this->lodManager.scaleRadius(pos, radii.polar, fov, viewportHeight, importance);
    object->scaleRadii(Radii{scaledEquatorianRadius, scaledPolarRadius, scaledMeanRadius});
    this->fullInstances.emplace_back(object);
    break;
  }
  case LOD::Impostor:
    this->impostorInstances.emplace_back(InstancePositionRadiusTexture{pos, scaledMeanRadius, this->textureLayers[texture->getId()]});
    break;
  case LOD::Point:
    this->pointInstances.emplace_back(InstancePositionRadiusColor{pos, scaledMeanRadius, texture->getAverageColor()});
    break;
  default:
    Logger::logError("Asteroid System", "No handler for LOD level: " + std::to_string(level));
    break;
  }
}

void Renderer::partitionObjects(Scene &scene, RenderContext &ctx)
{
  this->fullInstances.clear();
  this->impostorInstances.clear();
  this->pointInstances.clear();

  const Camera &camera = scene.getActiveCamera();
  Frustum frustum = scene.getActiveCamera().getFrustum(ctx.frameCtx.aspect);

  // change from getmodel source to get LOD/smth like that
  for (ModelSource *object : scene.getModelSources())
    this->partitionObject(object, &frustum, ctx.frameCtx.height, camera.getFOV());

  for (auto &system : scene.getPhysicsWorld().getSystems())
    system->partitionObjects(this->impostorInstances, this->pointInstances, camera, &this->lodManager, ctx.frameCtx.height, &frustum);

  this->impostorMesh->updateInstanceBuffer(this->impostorInstances.data(), this->impostorInstances.size());
  this->pointMesh->updateInstanceBuffer(this->pointInstances.data(), this->pointInstances.size());
}

// Constructor
Renderer::Renderer(ResourceManager &resourceManager) : resourceManager(resourceManager), postProcess(resourceManager), blur(resourceManager) {}

// Public functions
void Renderer::init(Scene &scene, RenderContext &ctx)
{
  this->lightManager = std::make_unique<LightManager>(scene);
  this->shadowManager = std::make_unique<ShadowManager>(scene);

  this->initShaderBuffer(&this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);
  this->blur.init(37, 6.2f, ctx.frameCtx, true, this->shadowRes);

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

  this->textRenderer.init();
  this->postProcess.init(ctx.frameCtx);

  this->initShaderUBOBindings();

  this->initLOD(scene);
}

void Renderer::render(Scene &scene, RenderContext &ctx)
{
  this->beginFrame(ctx);

  this->bindUBOs();

  this->renderPointShadow(scene);
  this->renderDirectionalShadow(scene);

  this->renderMoonsRadiance(scene);

  const Framebuffer &hdrFramebuffer = this->postProcess.getHDRFramebuffer();

  this->renderToFramebuffer(scene, hdrFramebuffer, ctx);

  if (ctx.settings.useHDR)
  {
    this->postProcess.process(ctx);

    this->blitDepthToDefault(hdrFramebuffer);
  }

  this->renderTrails(scene);
}

void Renderer::update(Scene &scene, RenderContext &ctx)
{
  scene.update(ctx);

  this->partitionObjects(scene, ctx);

  this->updateUBO(scene, ctx);
}

void Renderer::renderText(const std::string &text, float x, float y, float scale, glm::vec3 color)
{
  Shader &textShader = this->resourceManager.GetShader(Res::TEXT_SHADER);
  this->textRenderer.render(textShader, text, x, y, scale, color);
}

void Renderer::resize(FrameContext &ctx)
{
  this->postProcess.init(ctx);
}
