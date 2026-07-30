#pragma once

#include "resources/range.h"

#include "graphics/instanceLayouts.h"

#include "graphics/buffers/buffer.h"

#include "compute/clBuffer.h"

#include <vector>

class InstanceManager
{
private:
  std::vector<InstanceModelMatrixParts> fullInstances;
  std::vector<InstancePositionRadiusTexture> impostorInstances;
  std::vector<InstancePositionRadiusColor> pointInstances;

  CLBuffer fullInstancesBuffer;
  CLBuffer impostorInstancesBuffer;
  CLBuffer pointInstancesBuffer;

  std::unique_ptr<Buffer> fullInstancesVBO;
  std::unique_ptr<Buffer> impostorInstancesVBO;
  std::unique_ptr<Buffer> pointInstancesVBO;

public:
  InstanceManager() = default;
  ~InstanceManager() = default;

  void init(size_t totalObjects);
  void initGPU(cl_context ctx);

  void clear();

  void fillVBOs();

  Range add(const InstanceModelMatrixParts &data);
  Range add(const InstancePositionRadiusTexture &data);
  Range add(const InstancePositionRadiusColor &data);

  Range add(std::vector<InstanceModelMatrixParts> &&data);
  Range add(std::vector<InstancePositionRadiusTexture> &&data);
  Range add(std::vector<InstancePositionRadiusColor> &&data);

  Buffer &getFullInstancesVBO() { return *this->fullInstancesVBO; };
  Buffer &getImpostorInstancesVBO() { return *this->impostorInstancesVBO; };
  Buffer &getPointInstancesVBO() { return *this->pointInstancesVBO; };

  size_t getImpostorCount() { return this->impostorInstances.size(); };
  size_t getPointCount() { return this->pointInstances.size(); };
};