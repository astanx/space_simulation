#pragma once

#include <GL/glew.h>

class ScopedCullFace
{
private:
  GLint prevFace;
  GLboolean prevEnabled;

public:
  explicit ScopedCullFace(GLenum face = GL_FRONT);
  ~ScopedCullFace();
};