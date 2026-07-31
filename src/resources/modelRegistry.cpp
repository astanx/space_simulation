#include "resources/modelRegistry.h"

#include "debug/logger.h"

#include "graphics/model.h"

// Public functions
uint32_t ModelRegistry::registerModel(Model *model)
{
  auto it = this->models.find(model);
  if (it != this->models.end())
  {
    Logger::logWarning("Model Registry", "Model registered twice");
    return it->second;
  }

  uint32_t id = this->freeID++;
  model->setID(id);
  this->models[model] = id;

  return id;
}