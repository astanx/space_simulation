#pragma once

#include "physics/structs/radii.h"
#include "resources/transform.h"

class PointLight;
class Camera;

struct SharedDatabaseView
{
  virtual ~SharedDatabaseView() = default;

  virtual Transform getTransform(size_t idx, const Camera &camera) const = 0;
  virtual Radii getRadii(size_t idx) const = 0;
  virtual void moveLight(size_t idx, const Camera &camera, PointLight &light) const = 0;
};