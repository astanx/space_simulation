#pragma once

#include <GL/glew.h>

class ScopedDepthMask
{
private:
  GLboolean prevMask;

public:
  explicit ScopedDepthMask(GLboolean mask = GL_TRUE);
  ~ScopedDepthMask();
};