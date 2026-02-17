#pragma once

#include "graphics/buffers/buffer.h"

class ScopedBuffer
{
private:
  GLint prevBuffer = 0;
  GLenum target;

  static GLenum getBindingEnumForTarget(GLenum target);

public:
  explicit ScopedBuffer(const Buffer &buffer, GLenum target, bool saveState = false);
  explicit ScopedBuffer(const GLint &buffer, GLenum target, bool saveState = false);

  ~ScopedBuffer();
};