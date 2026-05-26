#pragma once

#include "render/renderConfig.h"

#include <glm/glm.hpp>

namespace RenderState
{
  inline void applyDepthFunc()
  {
    glDepthFunc(RendererConfig::Depth::Func);
  }

  inline void clearDepth()
  {
    glClearDepth(RendererConfig::Depth::ClearValue);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  inline void clearColor(glm::vec4 color)
  {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
  }
}