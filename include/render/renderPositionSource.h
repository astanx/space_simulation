#pragma once

#include <glm/glm.hpp>

class RenderPositionSource
{
protected:
  glm::vec3 renderPosition;

public:
  RenderPositionSource() = default;
  virtual ~RenderPositionSource() = default;

  void setRenderPosition(glm::vec3 pos) { this->renderPosition = pos; };
  const glm::vec3 getRenderPosition() const { return this->renderPosition; };
};