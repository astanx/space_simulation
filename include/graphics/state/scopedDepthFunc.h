#pragma once

#include <GL/glew.h>

class ScopedDepthFunc
{
private:
  GLint prevFunc;

public:
  explicit ScopedDepthFunc(GLint func = GL_LESS);
  ~ScopedDepthFunc();
};