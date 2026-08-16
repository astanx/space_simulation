#pragma once

#include "render/textRenderer.h"
#include "render/postProcess.h"
#include "render/gaussianBlur.h"
#include "render/renderContext.h"
#include "render/lod/manager/lodManager.h"
#include "render/instanceManager.h"
#include "render/renderFlags.h"
#include "render/queue/renderQueue.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/buffers/renderBuffer.h"
#include "graphics/buffers/buffer.h"

#include "scene/light/lightManager.h"

#include "scene/shadow/shadowManager.h"

#include <memory>
#include <GL/glew.h>

class Scene;
class ResourceManager;
class Shader;
class Renderable;
class ModelSource;
class RendererBackend;
struct Frustum;
struct InstancePositionRadiusTexture;
struct InstancePositionRadiusColor;

struct CameraGPU
{
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::vec4 camPosition;
};

class Renderer
{
private:
  std::unique_ptr<RendererBackend> backend;
  ResourceManager &resourceManager;
  TextRenderer textRenderer;
  PostProcess postProcess;
  GaussianBlur blur;
  RenderQueue queue;

  const GLuint shadowRes = 1024;
  const GLuint cubemapRes = 1024;

  void renderDirectionalShadow(Scene &scene);
  void renderShadowMap(Scene &scene, Shader &shader);
  void renderPointShadow(Scene &scene);
  void renderReflectanceRadiance(Scene &scene);
  void renderSkybox(Scene &scene, RenderContext &ctx);
  void renderObjects(Scene &scene);
  void renderObjectsQueue(std::vector<RenderBatch> &batches, Scene &scene, Shader &shader, Buffer *instanceVBO);
  void renderAtmospheres(Scene &scene);
  void renderTrails(Scene &scene);
  void renderImpostor(Scene &scene);
  void renderPoint(Scene &scene);

  void bindDummyReflector(Shader &shader);

  void renderToFramebuffer(Scene &scene, const Framebuffer &framebuffer, RenderContext &ctx);
  void blitDepthToDefault(const Framebuffer &framebuffer);

  void beginFrame(RenderContext &ctx);

public:
  Renderer(ResourceManager &resourceManager);
  ~Renderer();

  void render(Scene &scene, RenderContext &ctx);

  void renderText(const std::string &text, float x, float y, float scale, glm::vec3 color);

  void update(Scene &scene, RenderContext &ctx);

  void initCPUBackend(Scene& scene);
  void initGPUBackend(Scene& scene);
  void init(RenderContext &ctx);

  void resize(FrameContext &ctx);
};
