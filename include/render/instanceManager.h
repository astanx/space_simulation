#pragma once

#include "resources/range.h"

#include "graphics/instanceLayouts.h"

#include "graphics/buffers/buffer.h"

#include "compute/clBuffer.h"

#include <vector>
#include <memory>
#include <cstring>

class Model;

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

  std::vector<Range> allocations;
  size_t lastAllocation;

  size_t impostorCount;
  size_t pointCount;

public:
  InstanceManager() = default;
  ~InstanceManager() = default;

  void init(size_t totalObjects);
  void initGPU(cl_context ctx);

  void clear();

  void fillVBOs();

  Range reserve(const Model *model, size_t capacity);

  Range add(const Model *model, const InstanceModelMatrixParts &data);
  Range add(const InstancePositionRadiusTexture &data);
  Range add(const InstancePositionRadiusColor &data);

  Range add(const Model *model, std::vector<InstanceModelMatrixParts> &&data);
  Range add(std::vector<InstancePositionRadiusTexture> &&data);
  Range add(std::vector<InstancePositionRadiusColor> &&data);

  void setImpostorCount(size_t count) { this->impostorCount = count; };
  void setPointCount(size_t count) { this->pointCount = count; };

  Buffer &getFullInstancesVBO() { return *this->fullInstancesVBO; };
  Buffer &getImpostorInstancesVBO() { return *this->impostorInstancesVBO; };
  Buffer &getPointInstancesVBO() { return *this->pointInstancesVBO; };

  CLBuffer &getFullInstancesBuffer() { return this->fullInstancesBuffer; };
  CLBuffer &getImpostorInstancesBuffer() { return this->impostorInstancesBuffer; };
  CLBuffer &getPointInstancesBuffer() { return this->pointInstancesBuffer; };

  size_t getImpostorCount() { return this->impostorCount; };
  size_t getPointCount() { return this->pointCount; };

  Range getAllocation(const Model *model);
};