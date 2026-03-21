#pragma once

#include "resources/resourceManager.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/buffers/renderBuffer.h"

#include "render/gaussianBlur.h"

#include <memory>

class PostProcess
{
private:
  std::unique_ptr<Framebuffer> hdrFBO;
  std::unique_ptr<RenderBuffer> rboDepth;
  std::unique_ptr<Texture> hdrColorBufferTexture;

  ResourceManager &resourceManager;

  GaussianBlur blur;

  void renderFullscreenQuad(bool useBloom);

  void initHDR();

  void renderBloom();

public:
  PostProcess(ResourceManager &resourceManager);
  ~PostProcess() = default;

  void init();

  void process(bool useBloom);

  const Framebuffer &getHDRFramebuffer() { return *this->hdrFBO; };
};