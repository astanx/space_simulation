#pragma once

#include <GL/glew.h>

namespace RendererConfig
{
  namespace Depth
  {
    // basic depth buffer
    inline constexpr float ClearValue = 1.0f;

    inline constexpr GLenum Func = GL_LESS;
    inline constexpr GLenum SkyboxFunc = GL_LEQUAL;

    // reverse z-buffering (does not work)
    // inline constexpr float ClearValue = 0.0f;

    // inline constexpr GLenum Func = GL_GREATER;
    // inline constexpr GLenum SkyboxFunc = GL_GEQUAL;
  }
}