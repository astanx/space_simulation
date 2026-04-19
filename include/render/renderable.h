#pragma once

#include "physics/positionSource.h"

#include "graphics/model.h"

class Shader;

class Renderable
{
protected:
public:
  Renderable() = default;
  virtual ~Renderable() = default;

  virtual void render(Shader &shader) const = 0;
  virtual void renderInstanced(Shader &shader) const = 0;
};