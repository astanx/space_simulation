#pragma once

#include "GL/glew.h"

class Framebuffer
{
private:
  GLuint id = 0;

public:
  Framebuffer();
  ~Framebuffer();

  inline GLuint getId() const { return this->id; };

  void attachTexture(GLenum attachment, GLuint texture, GLint level);
  void attachTexture2D(GLenum attachment, GLenum target, GLuint texture, GLint level);
  void attachRenderBuffer(GLenum attachment, GLenum target, GLuint buffer);

  void bind(GLenum target = GL_FRAMEBUFFER) const;
  void unbind(GLenum target = GL_FRAMEBUFFER) const;

  bool checkComplete() const;
};