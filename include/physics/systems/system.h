#pragma once

#include <functional>

#include <glm/glm.hpp>

class Object;
class Camera;
class LODManager;
struct Frustum;

class System
{
protected:
public:
  System() = default;
  virtual ~System() = default;

  virtual void forEachObject(std::function<void(Object &)> &&func) = 0;
  virtual void partitionObjects(const Camera &camera, LODManager* manager, float viewportHeight, Frustum *frustum = nullptr, bool force = false) = 0;
};