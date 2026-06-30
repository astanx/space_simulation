#pragma once

#include "scene/frameContext.h"

struct Frustum;
class Camera;

class Updatable
{
protected:
public:
  Updatable() = default;
  virtual ~Updatable() = default;

  virtual void update(const Camera &camera, Frustum *frustum = nullptr, bool force = false) = 0;
};