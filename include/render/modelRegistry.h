#pragma once

#include <cstddef>
#include <map>

class Model;

class ModelRegistry
{
private:
  std::unordered_map<Model *, size_t> models;
  size_t freeID = 0;

public:
  size_t add(Model* model);
};