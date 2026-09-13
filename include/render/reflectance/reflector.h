#pragma once

#include "graphics/model.h"

#include "render/renderPositionSource.h"

class Reflector : public Model, public RenderPositionSource
{
private:
public:
  using Model::Model;
  ~Reflector() = default;
};