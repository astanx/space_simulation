#pragma once

#include "graphics/renderable.h"
#include "physics/object.h"

class RenderableObject : public Object, public Renderable
{
public:
  virtual ~RenderableObject() = default;

  void update(double dt) override;
  void render(Shader *shader) override;
};