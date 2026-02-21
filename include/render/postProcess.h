#pragma once

#include "resources/resourceManager.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/buffers/renderBuffer.h"

#include <memory>

class PostProcess
{
private:
  std::unique_ptr<Framebuffer> hdrFBO;
  std::unique_ptr<RenderBuffer> rboDepth;
  std::unique_ptr<Texture> hdrColorBufferTexture;
  std::unique_ptr<Mesh> fullscreenQuad;

  std::unique_ptr<Framebuffer> pingpongFBOs[2];
  std::unique_ptr<Texture> pingpongBuffers[2];
  Texture *finalBloomTexture;

  std::vector<float> weights;

  ResourceManager &resourceManager;

  void renderFullscreenQuad(bool useBloom);

  void initBloom();
  void initHDR();

  void renderBloom();
  void blurBloom();

  void sendWeights(Shader &shader);

public:
  PostProcess(ResourceManager &resourceManager);
  ~PostProcess() = default;

  void init();

  void process(bool useBloom);

  const Framebuffer &getHDRFramebuffer() { return *this->hdrFBO; };
};