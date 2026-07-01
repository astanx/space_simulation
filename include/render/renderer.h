#pragma once

#include "render/textRenderer.h"
#include "render/postProcess.h"
#include "render/gaussianBlur.h"
#include "render/renderContext.h"
#include "render/lodManager.h"

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
class Renderable;
class ModelSource;
struct Frustum;
struct InstancePositionRadiusTexture;
struct InstancePositionRadiusColor;

struct CameraGPU
{
  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::vec4 camPosition;
};

struct ScaleGPU
{
  float viewportHeight;
  float baseMinPixelSize;
  float fov;
};

class Renderer
{
private:
  ResourceManager &resourceManager;
  TextRenderer textRenderer;
  PostProcess postProcess;
  GaussianBlur blur;
  LODManager lodManager;

  bool isImpostorInitialized = false;
  std::unique_ptr<Texture> impostorTexture;
  std::unique_ptr<Mesh> impostorMesh;
  std::unique_ptr<Mesh> pointMesh;

  std::unordered_map<GLuint, unsigned int> textureLayers;

  std::vector<Renderable *> fullInstances;
  std::vector<InstancePositionRadiusTexture> impostorInstances;
  std::vector<InstancePositionRadiusColor> pointInstances;

  unsigned int cameraUBO;

  std::unique_ptr<LightManager> lightManager;

  const GLuint shadowRes = 1024;
  std::unique_ptr<ShadowManager> shadowManager;

  void updateUBO(Scene &scene, RenderContext &ctx);

  void initShaderBuffer(GLuint *ubo, unsigned long size, GLenum bufferType);

  void initLOD(Scene &scene);
  void initImpostor(Scene &scene);
  void initPoint();
  void bindLayerToImpostorTexture(const Texture &texture, unsigned int layer);
  void partitionObjects(Scene &scene, RenderContext &ctx);
  void partitionObject(ModelSource *object, Frustum *frustum, float viewportHeight, float fov, bool force = false);

  void renderDirectionalShadow(Scene &scene);
  void renderShadowMap(Scene &scene, Shader &shader);
  void renderPointShadow(Scene &scene);
  void renderMoonsRadiance(Scene &scene);
  void renderSkybox(Scene &scene, RenderContext &ctx);
  void renderAsteroidSystems(Scene &scene, Frustum *frustum);
  void renderObjects(Scene &scene, Frustum *frustum);
  void renderAtmospheres(Scene &scene, Frustum *frustum);
  void renderTrails(Scene &scene);
  void renderImpostor(Scene &scene);
  void renderPoint();

  void initShaderUBOBindings();
  void bindUBOs();

  void bindDummyReflector(Shader &shader);

  void renderToFramebuffer(Scene &scene, const Framebuffer &framebuffer, RenderContext &ctx);
  void blitDepthToDefault(const Framebuffer &framebuffer);

  void beginFrame(RenderContext &ctx);

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
