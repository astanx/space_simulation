#include "render/postProcess.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include "graphics/shader.h"

#include "graphics/primitives/quad.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedShader.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedFramebuffer.h"

// Private functions
void PostProcess::renderFullscreenQuad(bool useBloom)
{
  ScopedDepthMask depthMask(false);
  ScopedDepthTest depthTest(false);

  Shader &hdrShader = this->resourceManager.GetShader(Res::HDR_SHADER);

  ScopedShader hdr(hdrShader);

  ScopedTexture hdrColor(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);

  std::optional<ScopedTexture> bloom;
  if (useBloom)
    bloom.emplace(*this->blur.getFinalTexture(), TextureBindingPoints::Bloom);

  hdrShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  hdrShader.set1i(TextureBindingPoints::Bloom, "bloomBlur");
  hdrShader.set1f(5e-3f, "exposure");
  hdrShader.set1f(0.22f, "bloomPower");

  this->blur.renderFullscreenQuad();
}

void PostProcess::initHDR()
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  this->hdrFBO = std::make_unique<Framebuffer>();

  this->hdrColorBufferTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

  this->rboDepth = std::make_unique<RenderBuffer>();

  this->rboDepth->bind(GL_RENDERBUFFER);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  {
    ScopedFramebuffer hdr(*this->hdrFBO, GL_FRAMEBUFFER);

    this->hdrFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorBufferTexture->getId(), 0);

    this->hdrFBO->attachRenderBuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth->getId());

    this->hdrFBO->checkComplete();
  }
}

void PostProcess::renderBloom()
{
  ScopedFramebuffer pingpong(*this->blur.getPingPongFBO(0), GL_FRAMEBUFFER);
  glClear(GL_COLOR_BUFFER_BIT);

  Shader &bloomShader = this->resourceManager.GetShader(Res::BLOOM_SHADER);

  ScopedShader bloom(bloomShader);

  ScopedTexture hdrColorBuffer(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);

  bloomShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  bloomShader.set1f(1.f, "threshold");

  this->blur.renderFullscreenQuad();
}

// Constructor
PostProcess::PostProcess(ResourceManager &resourceManager) : resourceManager(resourceManager), blur(resourceManager)
{
}

void PostProcess::init()
{
  this->initHDR();
  this->blur.init(4, 4.f);
}

void PostProcess::process(bool useBloom)
{
  if (useBloom)
  {
    this->renderBloom();
    this->blur.blur(*this->blur.getPingPongBuffer(0), 5);
  }

  this->renderFullscreenQuad(useBloom);
}