#pragma once

#include "graphics/framebuffers/framebuffer.h"

class ScopedFramebuffer
{
private:
  GLint prevFBO = 0;
  GLenum target;

  static GLenum getBindingEnumForTarget(GLenum target);

public:
  explicit ScopedFramebuffer(const Framebuffer &buffer, GLenum target, bool saveState = false);
  explicit ScopedFramebuffer(const GLint &buffer, GLenum target, bool saveState = false);

  ~ScopedFramebuffer();
};