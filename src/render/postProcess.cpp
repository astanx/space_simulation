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
    bloom.emplace(*this->finalBloomTexture, TextureBindingPoints::BloomBlur);

  hdrShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  hdrShader.set1i(TextureBindingPoints::BloomBlur, "bloomBlur");
  hdrShader.set1f(0.09f, "exposure");
  hdrShader.set1f(0.22f, "bloomPower");

  this->fullscreenQuad->render();
}

void PostProcess::initBloom()
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  float width = static_cast<float>(viewport[2]);
  float height = static_cast<float>(viewport[3]);

  for (unsigned int i = 0; i < 2; i++)
  {
    this->pingpongFBOs[i] = std::make_unique<Framebuffer>();

    ScopedFramebuffer pingpong(*this->pingpongFBOs[i], GL_FRAMEBUFFER, true);

    this->pingpongBuffers[i] = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    this->pingpongFBOs[i]->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pingpongBuffers[i]->getId(), 0);

    this->pingpongFBOs[i]->checkComplete();
  }

  this->finalBloomTexture = this->pingpongBuffers[0].get();
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

  std::unique_ptr<Quad> obj = std::make_unique<Quad>();

  this->fullscreenQuad = std::make_unique<Mesh>(std::move(obj), VertexLayout::PositionTexcoord);
}

void PostProcess::renderBloom()
{
  ScopedFramebuffer pingpong(*this->pingpongFBOs[0], GL_FRAMEBUFFER);
  glClear(GL_COLOR_BUFFER_BIT);

  Shader &bloomShader = this->resourceManager.GetShader(Res::BLOOM_SHADER);

  ScopedShader bloom(bloomShader);

  ScopedTexture hdrColorBuffer(*this->hdrColorBufferTexture, TextureBindingPoints::HDRColorBuffer);

  bloomShader.set1i(TextureBindingPoints::HDRColorBuffer, "hdrBuffer");
  bloomShader.set1f(1.f, "threshold");

  this->fullscreenQuad->render();
}

void PostProcess::blurBloom()
{
  bool horizontal = true, first_iteration = true;
  int amount = 5;

  Shader &blurShader = this->resourceManager.GetShader(Res::BLUR_SHADER);

  ScopedShader blur(blurShader);

  this->sendWeights(blurShader);

  for (unsigned int i = 0; i < amount; i++)
  {
    ScopedFramebuffer pingpong(*this->pingpongFBOs[horizontal], GL_FRAMEBUFFER, true);

    std::unique_ptr<ScopedTexture> pingpongBuffScope;

    if (first_iteration)
      pingpongBuffScope = std::make_unique<ScopedTexture>(*this->pingpongBuffers[0], TextureBindingPoints::BloomBlur);
    else
      pingpongBuffScope = std::make_unique<ScopedTexture>(*this->pingpongBuffers[!horizontal], TextureBindingPoints::BloomBlur);

    blurShader.set1i(horizontal, "horizontal");
    blurShader.set1i(TextureBindingPoints::BloomBlur, "image");

    this->fullscreenQuad->render();

    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
  }

  this->finalBloomTexture = pingpongBuffers[horizontal].get();
}

void PostProcess::sendWeights(Shader &shader)
{
  shader.set1i(this->weights.size(), "kernel_size");

  shader.setVec1f(this->weights.data(), this->weights.size(), "weight");
}

float PostProcess::gaussian(float x, float mean, float stddev)
{
  const float a = (x - mean) / stddev;
  return std::exp(-0.5 * a * a);
}

std::vector<float> PostProcess::create_gaussian_blur_weights(size_t kernel_size, float std_dev)
{
  std::vector<float> gaussian_blur_weights(kernel_size, 0.0f);
  for (size_t i = 0; i < (kernel_size + 1) / 2; ++i)
    gaussian_blur_weights[i] = gaussian_blur_weights[kernel_size - 1 - i] = gaussian(i, (kernel_size - 1) / 2.0, std_dev);

  float weight_sum = 0.0;
  for (auto weight : gaussian_blur_weights)
    weight_sum += weight;

  for (size_t i = 0; i < (kernel_size + 1) / 2; ++i)
    gaussian_blur_weights[i] = gaussian_blur_weights[kernel_size - 1 - i] = gaussian_blur_weights[i] / weight_sum;

  return gaussian_blur_weights;
}

// Constructor
PostProcess::PostProcess(ResourceManager &resourceManager) : resourceManager(resourceManager)
{
  // Max kernel_size - 40
  this->weights = this->create_gaussian_blur_weights(4, 4.f);
}

void PostProcess::init()
{
  this->initBloom();
  this->initHDR();
}

void PostProcess::process(bool useBloom)
{
  if (useBloom)
  {
    this->renderBloom();
    this->blurBloom();
  }

  this->renderFullscreenQuad(useBloom);
}