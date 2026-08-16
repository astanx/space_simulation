#include "render/renderer/renderer.h"

#include "render/renderer/backend/rendererBackend.h"
#include "render/renderer/backend/rendererBackendGPU.h"
#include "render/renderer/backend/rendererBackendCPU.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "render/renderState.h"
#include "render/reflectanceAcceptor.h"
#include "render/frustum.h"
#include "render/queue/renderBatch.h"

#include "graphics/instanceLayouts.h"
#include "graphics/skybox.h"

#include "graphics/bindings/texture.h"
#include "graphics/bindings/impostor.h"

#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedDepthFunc.h"
#include "graphics/state/scopedCullFace.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedPolygonOffset.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedShader.h"

#include "graphics/buffers/renderBuffer.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "physics/star.h"

#include <iostream>

// Private functions
void Renderer::renderObjectsQueue(std::vector<RenderBatch> &batches, Scene &scene, Shader &shader, Buffer *instanceVBO)
{
  GLuint &ID = shader.getId();
  const Skybox &skybox = scene.getActiveSkybox();

  ScopedShader sh(ID);

  this->backend->bindPointShadow(shader);
  this->backend->bindPointShadowDepth(shader);

  skybox.bindIrradianceMap(shader);

  this->backend->bindDummyReflector(shader);

  size_t size = sizeof(InstanceModelMatrixParts);

  // Render all objects
  for (RenderBatch batch : batches)
  {
    std::optional<ScopedPolygonOffset> offset;
    std::optional<ScopedBlending> blend;
    std::optional<ScopedDepthMask> mask;

    if (batch.flag == RenderFlags::Main)
      offset.emplace(true, .1f, 4.f);

    if (batch.flag == RenderFlags::Layer)
    {
      blend.emplace(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      mask.emplace(GL_FALSE);
    }

    size_t count = batch.range.end - batch.range.begin;
    batch.model->renderInstanced(shader, instanceVBO, size, count, batch.range.begin);
  }

  skybox.unbindIrradianceMap();
}

void Renderer::renderObjects(Scene &scene)
{
  Shader &coreShader = this->resourceManager.GetShader(Res::CORE_SHADER);
  Shader &coreTangentShader = this->resourceManager.GetShader(Res::CORE_TANGENT_SHADER);

  this->renderObjectsQueue(this->queue.getCoreBatches(), scene, coreShader, &scene.getSimulationWorld().getRenderWorld().getFullInstancesVBO());
  this->renderObjectsQueue(this->queue.getTangentBatches(), scene, coreTangentShader, &scene.getSimulationWorld().getRenderWorld().getFullInstancesVBO());
}

// void Renderer::renderAtmospheres(Scene &scene)
// {
//   Shader &atmosphereShader = this->resourceManager.GetShader(Res::ATMOSPHERE_SHADER);

//   GLuint &atmosphereID = atmosphereShader.getId();

//   ScopedShader atmosphereSh(atmosphereID);

//   for (const Planet *planet : scene.getSimulationWorld().getPhysicsWorld().getPlanetarObjects())
//     planet->renderAtmosphere(atmosphereShader);
// }
void Renderer::renderTrails(Scene &scene)
{
  Shader &trailShader = this->resourceManager.GetShader(Res::TRAIL_SHADER);

  GLuint &trailID = trailShader.getId();

  ScopedShader trail(trailID);

  for (const Trail *trail : scene.getSimulationWorld().getRenderWorld().getTrails())
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
  size_t size = sizeof(InstanceModelMatrixParts);

  for (RenderBatch batch : this->queue.getShadowBatches())
  {
    size_t count = batch.range.end - batch.range.begin;
    batch.model->renderInstanced(shader, &scene.getSimulationWorld().getRenderWorld().getFullInstancesVBO(), size, count, batch.range.begin);
  }
}

void Renderer::renderDirectionalShadow(Scene &scene)
{
  if (!scene.getDirLight())
    return;

  ScopedViewport viewport(0, 0, this->shadowRes, this->shadowRes);
  ScopedFramebuffer dirShadowBuff(this->backend->getDirShadowFramebuffer(), GL_FRAMEBUFFER);

  RenderState::clearDepth();

  Shader &dirShadowShader = this->resourceManager.GetShader(Res::DIRECTIONAL_SHADOW_SHADER);

  ScopedShader dirShadowSd(dirShadowShader);

  this->renderShadowMap(scene, dirShadowShader);
}

void Renderer::renderPointShadow(Scene &scene)
{
  const PointLight *pointLight = scene.getPointLight();

  ScopedViewport viewport(0, 0, shadowRes, shadowRes);
  ScopedFramebuffer pointShadowBuff(this->backend->getPointShadowFramebuffer(), GL_FRAMEBUFFER);

  if (!pointLight)
    return;

  Shader &pointShadowShader = this->resourceManager.GetShader(Res::POINT_SHADOW_SHADER);

  GLuint &pointShadowID = pointShadowShader.getId();

  RenderState::clearColor(glm::vec4(exp(40.0f), 0, 0, 0));
  RenderState::clearDepth();

  ScopedShader pointShadowSd(pointShadowID);
  // ScopedCullFace cullFace(GL_FRONT);

  this->renderShadowMap(scene, pointShadowShader);

  this->blur.blur(this->backend->getPointShadowMapTexture(), 16, true);
}

void Renderer::renderImpostor(Scene &scene)
{
  Shader &impostorShader = this->resourceManager.GetShader(Res::IMPOSTOR_SHADER);
  GLuint &impostorID = impostorShader.getId();

  ScopedShader impostor(impostorID);
  ScopedTexture impostorText(scene.getSimulationWorld().getRenderWorld().getImpostorTexture(), ImpostorTextureBindingPoints::Impostor);

  const Skybox &skybox = scene.getActiveSkybox();
  skybox.bindIrradianceMap(impostorShader);

  impostorShader.set1i(ImpostorTextureBindingPoints::Impostor, "impostors");

  scene.getSimulationWorld().getRenderWorld().renderImpostorMeshInstanced();

  skybox.unbindIrradianceMap();
}

void Renderer::renderPoint(Scene &scene)
{
  Shader &pointShader = this->resourceManager.GetShader(Res::POINT_SHADER);
  GLuint &pointID = pointShader.getId();

  ScopedShader point(pointID);

  scene.getSimulationWorld().getRenderWorld().renderPointMeshInstanced();
}

void Renderer::renderToFramebuffer(Scene &scene, const Framebuffer &framebuffer, RenderContext &ctx)
{
  ScopedBlending blendingDisabled(false);
  std::optional<ScopedFramebuffer> framebufferScope;

  if (ctx.settings.useHDR)
    framebufferScope.emplace(framebuffer, GL_FRAMEBUFFER);

  RenderState::clearColor(ctx.settings.clearColor);
  RenderState::clearDepth();

  this->renderObjects(scene);
  this->renderImpostor(scene);
  this->renderPoint(scene);
  // temporary off
  //  this->renderAtmospheres(scene, &frustum);
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

void Renderer::renderReflectanceRadiance(Scene &scene)
{
  Shader &reflectanceShader = this->resourceManager.GetShader(Res::REFLECTION_SHADER);

  ScopedShader reflectance(reflectanceShader);
  reflectanceShader.set1f(scene.getSimulationWorld().getPhysicsWorld().getSun().getLuminosity(), "lightLuminocity");
  for (ReflectorBatch batch : this->queue.getReflectorBatches())
    batch.acceptor->renderRadianceInstanced(reflectanceShader, scene.getActiveCamera(), batch.reflector, &scene.getSimulationWorld().getRenderWorld().getFullInstancesVBO());
}

void Renderer::beginFrame(RenderContext &ctx)
{
  RenderState::clearColor(ctx.settings.clearColor);
  RenderState::clearDepth();
  glClear(GL_STENCIL_BUFFER_BIT);
}

// Constructor / Destructor
Renderer::Renderer(ResourceManager &resourceManager) : resourceManager(resourceManager), postProcess(resourceManager), blur(resourceManager) {}
Renderer::~Renderer() = default;

// Public functions
void Renderer::initCPUBackend(Scene &scene)
{
  this->backend = std::make_unique<RendererBackendCPU>();
  this->backend->init(scene, this->resourceManager, this->shadowRes);
}
void Renderer::initGPUBackend(Scene &scene)
{
  this->backend = std::make_unique<RendererBackendGPU>();
  this->backend->init(scene, this->resourceManager, this->shadowRes);
}
void Renderer::init(RenderContext &ctx)
{
  this->blur.init(37, 6.2f, ctx.frameCtx, true, this->cubemapRes);

  this->textRenderer.init();
  this->postProcess.init(ctx.frameCtx);
}

void Renderer::render(Scene &scene, RenderContext &ctx)
{
  this->beginFrame(ctx);

  this->backend->bindUBOs();

  this->renderPointShadow(scene);
  this->renderDirectionalShadow(scene);

  this->renderReflectanceRadiance(scene);

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
  scene.update(this->queue, ctx);
  this->backend->update(scene, ctx);
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
