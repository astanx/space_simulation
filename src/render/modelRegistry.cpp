#include "render/modelRegistry.h"

// Public functions
size_t ModelRegistry::add(Model *model)
{
  auto it = this->models.find(model);
  if (it == this->models.end())
  {
    this->models[model] = this->freeID;
    return this->freeID++;
  }
  else
    return it->second;
}
