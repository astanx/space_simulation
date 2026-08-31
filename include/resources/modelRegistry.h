#pragma once

#include <unordered_map>
#include <cstdint>

class Model;

class ModelRegistry
{
private:
  std::unordered_map<Model *, uint32_t> models;
  uint32_t freeID = 0;

public:
  ModelRegistry() = default;
  ~ModelRegistry() = default;

  uint32_t registerModel(Model *model);
};