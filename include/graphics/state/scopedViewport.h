#pragma once

#include <GL/glew.h>

class ScopedViewport
{
private:
  GLint prevViewport[4];

public:
  explicit ScopedViewport(GLint x = 0, GLint y = 0, GLsizei width = 800, GLsizei height = 600);
  ~ScopedViewport();
};