#include "render/instanceManager.h"

#include "debug/logger.h"

#include "graphics/state/scopedBuffer.h"

#include "graphics/instanceLayouts.h"

// Public functions
void InstanceManager::init()
{
  this->fullInstancesVBO = std::make_unique<Buffer>();
  this->impostorInstancesVBO = std::make_unique<Buffer>();
  this->pointInstancesVBO = std::make_unique<Buffer>();
}

void InstanceManager::clear()
{
  this->fullInstances.clear();
  this->impostorInstances.clear();
  this->pointInstances.clear();
}

void InstanceManager::fillVBOs()
{
  {
    ScopedBuffer buff(*this->fullInstancesVBO, GL_ARRAY_BUFFER);
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, this->fullInstances.size() * sizeof(InstanceModelMatrixParts), this->fullInstances.data(), GL_DYNAMIC_DRAW));
  }

  {
    ScopedBuffer buff(*this->impostorInstancesVBO, GL_ARRAY_BUFFER);
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, this->impostorInstances.size() * sizeof(InstancePositionRadiusTexture), this->impostorInstances.data(), GL_DYNAMIC_DRAW));
  }

  {
    ScopedBuffer buff(*this->pointInstancesVBO, GL_ARRAY_BUFFER);
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, this->pointInstances.size() * sizeof(InstancePositionRadiusColor), this->pointInstances.data(), GL_DYNAMIC_DRAW));
  }
}

Range InstanceManager::add(const InstanceModelMatrixParts &data)
{
  Range range;
  range.begin = this->fullInstances.size();
  this->fullInstances.push_back(data);
  range.end = this->fullInstances.size();

  if (range.begin != range.end - 1)
    Logger::logWarning("Instance Manager", "Full instance range error");

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

Range InstanceManager::add(std::vector<InstanceModelMatrixParts> &&data)
{
  size_t size = data.size();

  Range range;
  range.begin = this->fullInstances.size();
  this->fullInstances.insert(fullInstances.end(), std::make_move_iterator(data.begin()), std::make_move_iterator(data.end()));
  range.end = this->fullInstances.size();

  if (range.begin != range.end - size)
    Logger::logWarning("Instance Manager", "Full instance vector range error");

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