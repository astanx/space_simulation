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
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedBlending.h"

// Private functions
void PostProcess::renderFullscreenQuad(bool useBloom)
{
  ScopedDepthMask depthMask(false);
  ScopedDepthTest depthTest(false);

  Shader &hdrShader = this->resourceManager.GetShader(Res::HDR_SHADER);

  ScopedShader hdr(hdrShader);

  ScopedTexture hdrColor(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);
  ScopedTexture hdrEmissive(*this->hdrEmissiveBufferTexture, TextureBindingPoints::Emissive);

  std::optional<ScopedTexture> bloom;
  if (useBloom)
    // bloom.emplace(*this->blur.getFinalTexture(), TextureBindingPoints::Bloom);
    bloom.emplace(*this->mipChain[0].mipTexture, TextureBindingPoints::Bloom);

  hdrShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrColorBuffer");
  hdrShader.set1i(TextureBindingPoints::Emissive, "hdrEmissiveBuffer");
  hdrShader.set1i(TextureBindingPoints::Bloom, "bloomBlur");
  hdrShader.set1f(this->ctx->exposure, "exposure");
  hdrShader.set1f(0.12f, "bloomPower");
  this->resourceManager.GetMesh(Res::FULLSCREEN_QUAD).render();
}

void PostProcess::initHDR(float width, float height)
{
  this->hdrFBO = std::make_unique<Framebuffer>();

  this->hdrColorBufferTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);
  this->hdrEmissiveBufferTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

  this->rboDepth = std::make_unique<RenderBuffer>();

  this->rboDepth->bind(GL_RENDERBUFFER);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  {
    ScopedFramebuffer hdr(*this->hdrFBO, GL_FRAMEBUFFER);

    this->hdrFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->hdrColorBufferTexture->getId(), 0);
    this->hdrFBO->attachTexture2D(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->hdrEmissiveBufferTexture->getId(), 0);

    this->hdrFBO->attachRenderBuffer(GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth->getId());

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    this->hdrFBO->checkComplete();
  }
}

void PostProcess::initBloom(float width, float height)
{
  this->bloomExtractTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);
  this->bloomFBO = std::make_unique<Framebuffer>();

  {
    ScopedFramebuffer bloom(this->bloomFBO->getId(), GL_FRAMEBUFFER);

    this->bloomFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->bloomExtractTexture->getId(), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    this->bloomFBO->checkComplete();
  }
}

void PostProcess::initMip(unsigned int chainLength)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  this->mipFBO = std::make_unique<Framebuffer>();

  glm::vec2 mipSize = glm::vec2(width, height);

  for (size_t i = 0; i < chainLength; i++)
  {
    bloomMip mip;

    mipSize *= 0.5f;
    mip.mipSize = mipSize;
    mip.mipTexture = std::make_unique<Texture>(mipSize.x, mipSize.y, GL_TEXTURE_2D, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);

    this->mipChain.emplace_back(std::move(mip));
  }

  {
    ScopedFramebuffer mip(this->mipFBO->getId(), GL_FRAMEBUFFER);

    this->mipFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->mipChain[0].mipTexture->getId(), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    this->mipFBO->checkComplete();
  }
}

void PostProcess::extractBloom()
{
  ScopedFramebuffer bloom(*this->bloomFBO, GL_FRAMEBUFFER);

  glClear(GL_COLOR_BUFFER_BIT);

  Shader &bloomShader = this->resourceManager.GetShader(Res::BLOOM_SHADER);
  ScopedShader shader(bloomShader);

  ScopedTexture hdr(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);
  ScopedTexture hdrEmissive(*this->hdrEmissiveBufferTexture, TextureBindingPoints::Emissive);

  bloomShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrColorBuffer");
  bloomShader.set1i(TextureBindingPoints::Emissive, "hdrEmissiveBuffer");
  bloomShader.set1f(60000.f, "threshold");

  this->resourceManager.GetMesh(Res::FULLSCREEN_QUAD).render();
}

void PostProcess::downsampleBloom()
{
  Shader &downsampleShader = this->resourceManager.GetShader(Res::DOWNSAMPLE_SHADER);
  ScopedShader downsample(downsampleShader);
  ScopedFramebuffer mipbuff(this->mipFBO->getId(), GL_FRAMEBUFFER);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  downsampleShader.setVec2f(glm::vec2(width, height), "resolution");

  GLuint currentText = this->bloomExtractTexture->getId();

  downsampleShader.set1i(TextureBindingPoints::Bloom, "image");

  for (auto &mip : this->mipChain)
  {
    ScopedTexture text(static_cast<GLuint>(currentText), static_cast<GLenum>(GL_TEXTURE_2D), TextureBindingPoints::Bloom);

    ScopedViewport view(0, 0, mip.mipSize.x, mip.mipSize.y);

    this->mipFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.mipTexture->getId(), 0);

    this->resourceManager.GetMesh(Res::FULLSCREEN_QUAD).render();

    downsampleShader.setVec2f(mip.mipSize, "resolution");

    currentText = mip.mipTexture->getId();
  }
}

void PostProcess::upsampleBloom()
{
  Shader &upsampleShader = this->resourceManager.GetShader(Res::UPSAMPLE_SHADER);
  ScopedShader upsample(upsampleShader);
  ScopedFramebuffer mipbuff(this->mipFBO->getId(), GL_FRAMEBUFFER);

  ScopedBlending blend(true, GL_ONE, GL_ONE);

  upsampleShader.set1f(0.005f, "filterRadius");

  for (int i = this->mipChain.size() - 1; i > 0; i--)
  {
    bloomMip &current = this->mipChain[i];
    bloomMip &next = this->mipChain[i - 1];

    ScopedTexture text(*current.mipTexture, TextureBindingPoints::Bloom);
    ScopedViewport view(0, 0, next.mipSize.x, next.mipSize.y);

    upsampleShader.set1i(TextureBindingPoints::Bloom, "image");

    this->mipFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, next.mipTexture->getId(), 0);

    this->resourceManager.GetMesh(Res::FULLSCREEN_QUAD).render();
  }
}

void PostProcess::renderBloom()
{
  this->extractBloom();
  this->downsampleBloom();
  this->upsampleBloom();
  // ScopedFramebuffer pingpong(*this->blur.getPingPongFBO(0), GL_FRAMEBUFFER);
  // glClear(GL_COLOR_BUFFER_BIT);

  // Shader &bloomShader = this->resourceManager.GetShader(Res::BLOOM_SHADER);

  // ScopedShader bloom(bloomShader);

  // ScopedTexture hdrColorBuffer(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);

  // bloomShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrColorBuffer");
  // bloomShader.set1f(1.f, "threshold");

  // this->blur.renderFullscreenQuad();
}

// Constructor
PostProcess::PostProcess(ResourceManager &resourceManager) : resourceManager(resourceManager), blur(resourceManager)
{
}

void PostProcess::init(FrameContext *ctx)
{
  this->ctx = ctx;

  this->initHDR(ctx->width, ctx->height);
  this->initBloom(ctx->width, ctx->height);
  this->initMip(6);
  this->blur.init(4, 4.f);
}

void PostProcess::process(bool useBloom)
{
  if (useBloom)
  {
    this->renderBloom();
    // this->blur.blur(*this->blur.getPingPongBuffer(0), 20);
  }

  this->renderFullscreenQuad(useBloom);
}