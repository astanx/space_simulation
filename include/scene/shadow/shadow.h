#pragma once

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/texture.h"

#include <GL/glew.h>

#include <memory>

class Shader;

class Shadow
{
protected:
  std::unique_ptr<Framebuffer> shadowMapFBO;
  std::unique_ptr<Texture> shadowMapTexture;
  GLuint shadowWidth;
  GLuint shadowHeight;

  virtual void init() = 0;

public:
  Shadow(const GLuint width, const GLuint height);
  virtual ~Shadow() = default;

  virtual void bind(Shader &shader, int textureUnit, const std::string &name = "") const = 0;

  GLuint getShadowWidth() const { return this->shadowWidth; }
  GLuint getShadowHeight() const { return this->shadowHeight; }
  const Framebuffer &getShadowFramebuffer() const { return *this->shadowMapFBO; }
  const Texture &getShadowMapTexture() const { return *this->shadowMapTexture; }
};