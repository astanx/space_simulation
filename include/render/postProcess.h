#pragma once

#include "resources/resourceManager.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/buffers/renderBuffer.h"

#include "render/gaussianBlur.h"
#include "render/renderContext.h"

#include <memory>

struct bloomMip
{
  glm::vec2 mipSize;
  std::unique_ptr<Texture> mipTexture;
};

class PostProcess
{
private:
  std::unique_ptr<Framebuffer> hdrFBO;
  std::unique_ptr<RenderBuffer> rboDepth;
  std::unique_ptr<Texture> hdrColorBufferTexture;
  std::unique_ptr<Texture> hdrEmissiveBufferTexture;

  std::unique_ptr<Texture> bloomExtractTexture;
  std::unique_ptr<Framebuffer> bloomFBO;

  std::unique_ptr<Framebuffer> mipFBO;
  std::vector<bloomMip> mipChain;

  ResourceManager &resourceManager;

  GaussianBlur blur;

  void renderFullscreenQuad(RenderContext &ctx);

  void initHDR(float width, float height);
  void initBloom(float width, float height);
  void initMip(unsigned int chainLength, float width, float height);

  void downsampleBloom();
  void upsampleBloom();
  void extractBloom(RenderContext &ctx);
  void renderBloom(RenderContext &ctx);

public:
  PostProcess(ResourceManager &resourceManager);
  ~PostProcess() = default;

  void init(FrameContext &ctx);

  void process(RenderContext &ctx);

  const Framebuffer &getHDRFramebuffer() { return *this->hdrFBO; };
};