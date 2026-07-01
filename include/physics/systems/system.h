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

public:
  System() = default;
  virtual ~System() = default;

  virtual void forEachObject(std::function<void(Object &)> &&func) = 0;
  virtual void partitionObjects(std::vector<InstancePositionRadiusTexture> &impostorInstances, std::vector<InstancePositionRadiusColor> &pointInstances, const Camera &camera, LODManager *manager, float viewportHeight, Frustum *frustum = nullptr, bool force = false) = 0;

  unsigned int getImpostorLayer() { return this->impostorLayer; };
  void setImpostorLayer(unsigned int layer) { this->impostorLayer = layer; };

  virtual const Texture *getTexture() = 0;
};