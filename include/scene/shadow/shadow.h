#pragma once

#include "graphics/buffers/framebuffer.h"

#include <GL/glew.h>

#include <memory>

class Shader;

class Shadow
{
protected:
  std::unique_ptr<Framebuffer> shadowMapFBO;
  GLuint shadowMapTexture;
  GLuint shadowWidth;
  GLuint shadowHeight;

  virtual void init() = 0;

public:
  Shadow(const GLuint width, const GLuint height);
  virtual ~Shadow();

  virtual void bind(Shader &shader, int textureUnit) const = 0;

  GLuint getShadowWidth() const { return this->shadowWidth; }
  GLuint getShadowHeight() const { return this->shadowHeight; }
  const Framebuffer &getShadowFramebuffer() const { return *this->shadowMapFBO; }
};