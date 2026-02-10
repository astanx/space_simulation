#pragma once

#include <GL/glew.h>

class Shader;

class Shadow
{
protected:
  GLuint shadowMapFBO;
  GLuint shadowMapTexture;
  GLuint shadowWidth;
  GLuint shadowHeight;

  virtual void init() = 0;

public:
  Shadow(const GLuint width, const GLuint height);
  virtual ~Shadow();

  virtual void bindShadowMapFBO() const = 0;
  virtual void bind(Shader &shader, int textureUnit) const = 0;

  void unbindShadowMapFBO() const;

  GLuint getShadowWidth() const { return this->shadowWidth; }
  GLuint getShadowHeight() const { return this->shadowHeight; }
};