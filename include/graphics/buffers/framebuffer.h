#pragma once

#include "graphics/buffers/buffer.h"

class Framebuffer : public Buffer
{
protected:
public:
  Framebuffer();

  ~Framebuffer();

  void attachTexture(GLenum attachment, GLuint texture, GLint level);
  void attachTexture2D(GLenum attachment, GLenum target, GLuint texture, GLint level);
  void attachRenderBuffer(GLenum attachment, GLenum target, GLuint buffer);

  void bind(GLenum target = GL_FRAMEBUFFER) const override;
  void unbind(GLenum target = GL_FRAMEBUFFER) const override;

  bool checkComplete() const override;
};