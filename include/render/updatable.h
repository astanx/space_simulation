#pragma once

#include "scene/frameContext.h"

struct Frustum;

class Updatable
{
protected:
public:
  Updatable() = default;
  virtual ~Updatable() = default;

  virtual void update(double dt, FrameContext &ctx, Frustum *frustum = nullptr, bool force = false) = 0;
};