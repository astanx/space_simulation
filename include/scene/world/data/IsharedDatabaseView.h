#pragma once

#include <stddef.h>
#include <glm/glm.hpp>

class PointLight;
class Camera;
class CLBuffer;
struct Transform;
struct Radii;

struct ISharedDatabaseView
{
  virtual ~ISharedDatabaseView() = default;

  virtual glm::vec3 getPosition(size_t idx) const = 0;
  virtual Transform getTransform(size_t idx, const Camera &camera) const = 0;
  virtual Radii getRadii(size_t idx) const = 0;
  virtual void moveLight(size_t idx, const Camera &camera, PointLight &light) const = 0;
  virtual const CLBuffer &getPositionsBuffer() const = 0;
  virtual size_t getPositionsSize() const = 0;
};