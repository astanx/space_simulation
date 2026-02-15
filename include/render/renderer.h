#pragma once

#include "render/textRenderer.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "scene/light/lightManager.h"

#include "scene/shadow/shadowManager.h"

#include <memory>
#include <GL/glew.h>

class Scene;
class ResourceManager;
class Shader;

class Renderer
{
private:
  ResourceManager &resourceManager;
  TextRenderer textRenderer;

  unsigned int cameraUBO;

  std::unique_ptr<LightManager> lightManager;

  const GLuint shadowRes = 4096;
  std::unique_ptr<ShadowManager> shadowManager;

  // HDR
  GLuint hdrFBO = 0;
  GLuint rboDepth = 0;
  std::unique_ptr<Texture> hdrColorBufferTexture;
  std::unique_ptr<Mesh> fullscreenQuad;

  std::array<GLuint, 2> pingpongFBOs;
  std::array<std::unique_ptr<Texture>, 2> pingpongBuffers;

  void updateUBO(Scene &scene, float aspectRatio);
  void bindCameraUBO(GLuint programID);

  void initShaderBuffer(GLuint *ubo, unsigned long size, GLenum bufferType);

  void renderDirectionalShadow(Scene &scene);
  void renderShadowMap(Scene &scene, Shader &shader);
  void renderPointShadow(Scene &scene);
  void renderSkybox(Scene &scene);
  void renderAsteroidSystems(Scene &scene);
  void renderObjects(Scene &scene);
  void renderTrails(Scene &scene);

  void initHDR();
  void renderFullscreenQuad();
  void bindHDRFBO();
  void unbindHDRFBO();

  void initBloom();
  void renderBloom();
  void blurBloom();

public:
  Renderer(ResourceManager &resourceManager);
  ~Renderer() = default;

  void render(Scene &scene);

  void renderText(
      const std::string &text,
      float x, float y,
      float scale,
      glm::vec3 color);

  void update(Scene &scene, double dt, bool paused);
  void init(Scene &scene);
};
