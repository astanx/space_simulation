#pragma once

#include "render/textRenderer.h"
#include "render/postProcess.h"
#include "render/gaussianBlur.h"
#include "render/renderContext.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/buffers/renderBuffer.h"

#include "scene/light/lightManager.h"

#include "scene/shadow/shadowManager.h"

#include <memory>
#include <GL/glew.h>

class Scene;
class ResourceManager;
class Shader;
struct Frustum;

class Renderer
{
private:
  ResourceManager &resourceManager;
  TextRenderer textRenderer;
  PostProcess postProcess;
  GaussianBlur blur;

  unsigned int cameraUBO;

  std::unique_ptr<LightManager> lightManager;

  const GLuint shadowRes = 4096;
  std::unique_ptr<ShadowManager> shadowManager;

  void updateUBO(Scene &scene, RenderContext &ctx);

  void initShaderBuffer(GLuint *ubo, unsigned long size, GLenum bufferType);

  void renderDirectionalShadow(Scene &scene);
  void renderShadowMap(Scene &scene, Shader &shader);
  void renderPointShadow(Scene &scene);
  void renderMoonsRadiance(Scene &scene);
  void renderSkybox(Scene &scene, RenderContext &ctx);
  void renderAsteroidSystems(Scene &scene, Frustum *frustum);
  void renderObjects(Scene &scene, Frustum *frustum);
  void renderTrails(Scene &scene);

  void initShaderUBOBindings();
  void bindUBOs();

  void bindDummyReflector(Shader &shader);

  void renderToFramebuffer(Scene &scene, const Framebuffer &framebuffer, RenderContext &ctx);
  void blitDepthToDefault(const Framebuffer &framebuffer);

public:
  Renderer(ResourceManager &resourceManager);
  ~Renderer() = default;

  void render(Scene &scene, RenderContext &ctx);

  void renderText(
      const std::string &text,
      float x, float y,
      float scale,
      glm::vec3 color);

  void update(Scene &scene, RenderContext &ctx);
  void init(Scene &scene, RenderContext &ctx);

  void resize(FrameContext &ctx);
};
