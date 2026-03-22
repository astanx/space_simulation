#pragma once

#include "resources/resourceManager.h"

#include "graphics/texture.h"
#include "graphics/mesh.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/buffers/renderBuffer.h"

#include <memory>

class GaussianBlur
{
private:
  std::vector<float> weights;
  std::unique_ptr<Mesh> fullscreenQuad;

  ResourceManager &resourceManager;

  Texture *finalTexture;

  std::unique_ptr<Framebuffer> pingpongFBOs[2];
  std::unique_ptr<Texture> pingpongBuffers[2];

  void initBuffersFor2D();
  void initBuffersForCubemap(int resolution = 1024);
  void sendWeights(Shader &shader);

  float gaussian(float x, float mean, float stddev);
  std::vector<float> createGaussianBlurWeights(size_t kernel_size, float stddev);

public:
  GaussianBlur(ResourceManager &resourceManager);

  void init(size_t kernelSize, float stddev, bool isCube = false, int cubemapResolution = 1024);

  ~GaussianBlur() = default;

  void blur(const Texture &source, size_t amount, bool isCube = false);
  
  void renderFullscreenQuad() { this->fullscreenQuad->render(); };
  Framebuffer *getPingPongFBO(size_t index) { return this->pingpongFBOs[index].get(); };
  Texture *getPingPongBuffer(size_t index) { return this->pingpongBuffers[index].get(); };
  Texture *getFinalTexture() { return this->finalTexture; };
};