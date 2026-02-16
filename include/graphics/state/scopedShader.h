#pragma once

#include "graphics/shader.h"

#include <GL/glew.h>

class ScopedShader
{
private:
  GLint prevShader = 0;

public:
  explicit ScopedShader(const Shader &shader, bool saveState = false);
  explicit ScopedShader(const GLint program, bool saveState = false);
  ~ScopedShader();
};