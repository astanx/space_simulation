#pragma once

#include <functional>

#include <glm/glm.hpp>

class Object;
class Camera;
class LODManager;
class Texture;
struct Frustum;
struct InstancePositionRadiusColor;
struct InstancePositionRadiusTexture;

class System
{
protected:
  unsigned int impostorLayer;
  size_t totalObjects;

  virtual void forEachObjectImpl(std::function<void(Object &)> func) = 0;
  virtual void forEachObjectImpl(std::function<void(Object &, size_t)> func) = 0;

public:
  System() = default;
  virtual ~System() = default;

  template <typename F>
  void forEachObject(F &&func)
  {
    if constexpr (std::is_invocable_v<std::decay_t<F>, Object &, size_t>)
      forEachObjectImpl(std::function<void(Object &, size_t)>(std::forward<F>(func)));
    else if constexpr (std::is_invocable_v<std::decay_t<F>, Object &>)
      forEachObjectImpl(std::function<void(Object &)>(std::forward<F>(func)));
  }

  virtual void partitionObjects(std::vector<InstancePositionRadiusTexture> &impostorInstances, std::vector<InstancePositionRadiusColor> &pointInstances, const Camera &camera, LODManager *manager, float viewportHeight, Frustum *frustum = nullptr, bool force = false) = 0;

  unsigned int getImpostorLayer() { return this->impostorLayer; };
  size_t getTotalObjects() { return this->totalObjects; };
  void setImpostorLayer(unsigned int layer) { this->impostorLayer = layer; };

  virtual const Texture *getTexture() = 0;
};