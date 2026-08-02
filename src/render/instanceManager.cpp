#include "render/instanceManager.h"

#include "debug/logger.h"

#include "graphics/state/scopedBuffer.h"

#include "graphics/instanceLayouts.h"
#include "graphics/model.h"

// Public functions
void InstanceManager::init(size_t totalObjects)
{
  this->fullInstances.resize(totalObjects);

  this->fullInstancesVBO = std::make_unique<Buffer>();
  this->impostorInstancesVBO = std::make_unique<Buffer>();
  this->pointInstancesVBO = std::make_unique<Buffer>();

  {
    ScopedBuffer buff(*this->fullInstancesVBO, GL_ARRAY_BUFFER);
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, totalObjects * sizeof(InstanceModelMatrixParts), nullptr, GL_DYNAMIC_DRAW));
  }

  {
    ScopedBuffer buff(*this->impostorInstancesVBO, GL_ARRAY_BUFFER);
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, totalObjects * sizeof(InstancePositionRadiusTexture), nullptr, GL_DYNAMIC_DRAW));
  }

  {
    ScopedBuffer buff(*this->pointInstancesVBO, GL_ARRAY_BUFFER);
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, totalObjects * sizeof(InstancePositionRadiusColor), nullptr, GL_DYNAMIC_DRAW));
  }
}

void InstanceManager::initGPU(cl_context ctx)
{
  this->fullInstancesBuffer.init(ctx, CL_MEM_READ_WRITE, this->fullInstancesVBO->getId());
  this->pointInstancesBuffer.init(ctx, CL_MEM_READ_WRITE, this->pointInstancesVBO->getId());
  this->impostorInstancesBuffer.init(ctx, CL_MEM_READ_WRITE, this->impostorInstancesVBO->getId());
}

void InstanceManager::clear()
{
  this->impostorInstances.clear();
  this->pointInstances.clear();
}

void InstanceManager::fillVBOs()
{
  {
    ScopedBuffer buff(*this->fullInstancesVBO, GL_ARRAY_BUFFER);
    void *ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, this->fullInstances.size() * sizeof(InstanceModelMatrixParts), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

    if (!ptr)
      Logger::logFatal("Instance Manager", "Failed to map full instances");

    memcpy(ptr, this->fullInstances.data(), this->fullInstances.size() * sizeof(InstanceModelMatrixParts));
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  {
    ScopedBuffer buff(*this->impostorInstancesVBO, GL_ARRAY_BUFFER);
    void *ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, this->impostorInstances.size() * sizeof(InstancePositionRadiusTexture), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

    if (!ptr)
      Logger::logFatal("Instance Manager", "Failed to map impostor instances");

    memcpy(ptr, this->impostorInstances.data(), this->impostorInstances.size() * sizeof(InstancePositionRadiusTexture));
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  {
    ScopedBuffer buff(*this->pointInstancesVBO, GL_ARRAY_BUFFER);
    void *ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, this->pointInstances.size() * sizeof(InstancePositionRadiusColor), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

    if (!ptr)
      Logger::logFatal("Instance Manager", "Failed to map point instances");

    memcpy(ptr, this->pointInstances.data(), this->pointInstances.size() * sizeof(InstancePositionRadiusColor));
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }
}

Range InstanceManager::reserve(const Model *model, size_t capacity)
{
  if (this->allocations.size() <= model->getID())
    this->allocations.resize(model->getID() + 1);

  if (this->allocations[model->getID()].end != 0)
    Logger::logFatal("Instance Manager", "Same model reserved twice");

  this->allocations[model->getID()] = Range{this->lastAllocation, this->lastAllocation + capacity};

  this->lastAllocation += capacity;

  return this->allocations[model->getID()];
}

Range InstanceManager::add(const Model *model, const InstanceModelMatrixParts &data)
{
  if (this->allocations[model->getID()].end == 0)
    Logger::logFatal("Instance Manager", "Model must be reserved to add full instance");

  Range range;
  range.begin = this->allocations[model->getID()].begin;
  this->fullInstances[range.begin] = data;
  range.end = this->allocations[model->getID()].begin + 1;

  if (range.end > this->allocations[model->getID()].end)
    Logger::logFatal("Instance Manager", "Full instance range error");

  return range;
}
Range InstanceManager::add(const InstancePositionRadiusTexture &data)
{
  Range range;
  range.begin = this->impostorInstances.size();
  this->impostorInstances.push_back(data);
  range.end = this->impostorInstances.size();

  if (range.begin != range.end - 1)
    Logger::logWarning("Instance Manager", "Impostor instance range error");

  return range;
}
Range InstanceManager::add(const InstancePositionRadiusColor &data)
{
  Range range;
  range.begin = this->pointInstances.size();
  this->pointInstances.push_back(data);
  range.end = this->pointInstances.size();

  if (range.begin != range.end - 1)
    Logger::logWarning("Instance Manager", "Point instance range error");

  return range;
}

Range InstanceManager::add(const Model *model, std::vector<InstanceModelMatrixParts> &&data)
{
  if (model->getID() >= this->allocations.size() || this->allocations[model->getID()].end == 0)
    Logger::logFatal("Instance Manager", "Model must be reserved to add full instances");

  size_t size = data.size();

  Range range;
  range.begin = this->allocations[model->getID()].begin;
  std::move(data.begin(), data.end(), this->fullInstances.begin() + range.begin);
  range.end = this->allocations[model->getID()].begin + size;

  if (range.end > this->allocations[model->getID()].end)
    Logger::logFatal("Instance Manager", "Full instances range error");

  return range;
}
Range InstanceManager::add(std::vector<InstancePositionRadiusTexture> &&data)
{
  size_t size = data.size();

  Range range;
  range.begin = this->impostorInstances.size();
  this->impostorInstances.insert(impostorInstances.end(), std::make_move_iterator(data.begin()), std::make_move_iterator(data.end()));
  range.end = this->impostorInstances.size();

  if (range.begin != range.end - size)
    Logger::logWarning("Instance Manager", "Impostor instance vector range error");

  return range;
}
Range InstanceManager::add(std::vector<InstancePositionRadiusColor> &&data)
{
  size_t size = data.size();

  Range range;
  range.begin = this->pointInstances.size();
  this->pointInstances.insert(pointInstances.end(), std::make_move_iterator(data.begin()), std::make_move_iterator(data.end()));
  range.end = this->pointInstances.size();

  if (range.begin != range.end - size)
    Logger::logWarning("Instance Manager", "Point instance vector range error");

  return range;
}