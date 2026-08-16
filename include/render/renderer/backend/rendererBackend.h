#pragma once

#include "scene/light/lightManager.h"
#include "scene/shadow/shadowManager.h"

class Scene;
class ResourceManager;
struct RenderContext;

class RendererBackend
{
protected:
  LightManager lightManager;
  ShadowManager shadowManager;
  Buffer cameraUBO;

  void initShaderUBOBindings(ResourceManager &manager);
  void initShaderBuffer(Buffer &ubo, unsigned long size, GLenum bufferType);

  void updateCameraUBO(Scene &scene, RenderContext &ctx);

public:
  RendererBackend();
  virtual ~RendererBackend();

  void init(Scene &scene, ResourceManager &manager, size_t shadowRes);
  virtual void update(Scene &scene, RenderContext &ctx) = 0;

  void bindUBOs();
  void bindDummyReflector(Shader &shader);
  void bindPointShadow(Shader& shader, const std::string &name = "");
  void bindPointShadowDepth(Shader& shader, const std::string &name = "");
  const Framebuffer &getDirShadowFramebuffer();
  const Framebuffer &getPointShadowFramebuffer();
  const Texture &getPointShadowMapTexture();
};