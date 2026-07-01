#pragma once

#include "graphics/model.h"

#include "render/frustum.h"

class Shader;

class Renderable
{
protected:
public:
  Renderable() = default;
  virtual ~Renderable() = default;

  virtual void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const = 0;
  virtual void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const = 0;
};