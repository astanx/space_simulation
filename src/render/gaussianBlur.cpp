#include "render/gaussianBlur.h"

#include "resources/resources.h"

#include "graphics/bindings/texture.h"

#include "graphics/primitives/quad.h"

#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedShader.h"

// Private functions
float GaussianBlur::gaussian(float x, float mean, float stddev)
{
  const float a = (x - mean) / stddev;
  return std::exp(-0.5 * a * a);
}

std::vector<float> GaussianBlur::createGaussianBlurWeights(size_t kernel_size, float std_dev)
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

void GaussianBlur::initBuffers()
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

  this->finalTexture = this->pingpongBuffers[0].get();
}

// Constructor
GaussianBlur::GaussianBlur(ResourceManager &resourceManager) : resourceManager(resourceManager)
{
}

void GaussianBlur::init(size_t kernelSize, float stddev)
{
  this->weights = createGaussianBlurWeights(kernelSize, stddev);
  this->fullscreenQuad = std::make_unique<Mesh>(std::make_unique<Quad>(), VertexLayout::PositionTexcoord);

  this->initBuffers();
}

void GaussianBlur::sendWeights(Shader &shader)
{
  shader.set1i(this->weights.size(), "kernel_size");

  shader.setVec1f(this->weights.data(), this->weights.size(), "weight");
}

void GaussianBlur::blur(const Texture &source, size_t amount)
{
  bool horizontal = true;
  bool first_iteration = true;

  Shader &blurShader = this->resourceManager.GetShader(Res::BLUR_SHADER);
  ScopedShader blurSh(blurShader);

  this->sendWeights(blurShader);

  for (size_t i = 0; i < amount; i++)
  {
    ScopedFramebuffer pingpong(*this->pingpongFBOs[horizontal], GL_FRAMEBUFFER, true);

    std::unique_ptr<ScopedTexture> pingpongBuffScope;

    if (first_iteration)
      pingpongBuffScope = std::make_unique<ScopedTexture>(source, TextureBindingPoints::Blur);
    else
      pingpongBuffScope = std::make_unique<ScopedTexture>(*this->pingpongBuffers[!horizontal], TextureBindingPoints::Blur);

    blurShader.set1i(horizontal, "horizontal");
    blurShader.set1i(TextureBindingPoints::Blur, "image");

    this->fullscreenQuad->render();

    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
  }

  this->finalTexture = pingpongBuffers[horizontal].get();
}