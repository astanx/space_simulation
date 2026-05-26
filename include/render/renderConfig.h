#pragma once

#include <GL/glew.h>

namespace RendererConfig
{
  namespace Depth
  {
    inline constexpr float ClearValue = 1.0f;

    inline constexpr GLenum Func = GL_LESS;
  }
}