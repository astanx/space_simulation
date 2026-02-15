#pragma once

#include <GL/glew.h>

class ScopedBlending
{
private:
  GLboolean wasEnabled;
  GLint oldSrc;
  GLint oldDst;

public:
  explicit ScopedBlending(bool enable = true,
                          GLenum srcFactor = GL_SRC_ALPHA,
                          GLenum dstFactor = GL_ONE_MINUS_SRC_ALPHA);
  ~ScopedBlending();
};