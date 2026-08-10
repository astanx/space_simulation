#pragma once

#include "resources/gpuTypes.h"

#include <glm/glm.hpp>

#include <vector>

class Model;

struct RenderDataGPU
{
  std::vector<Vec3<float>> instanceColors;
  std::vector<uint32_t> instanceTextureLayers;
  std::vector<float> instanceImportances;
  std::vector<uint32_t> modelRangeStart;
  std::vector<uint32_t> modelRangeEnd;
  std::vector<uint32_t> isNonFullable;
  std::vector<Model *> models;

  void resize(size_t n)
  {
    this->instanceColors.resize(n);
    this->instanceTextureLayers.resize(n);
    this->instanceImportances.resize(n);
  }

  void combine(RenderDataGPU &data)
  {
    this->instanceColors.insert(this->instanceColors.end(), std::make_move_iterator(data.instanceColors.begin()), std::make_move_iterator(data.instanceColors.end()));
    this->instanceTextureLayers.insert(this->instanceTextureLayers.end(), std::make_move_iterator(data.instanceTextureLayers.begin()), std::make_move_iterator(data.instanceTextureLayers.end()));
    this->instanceImportances.insert(this->instanceImportances.end(), std::make_move_iterator(data.instanceImportances.begin()), std::make_move_iterator(data.instanceImportances.end()));
    this->modelRangeStart.insert(this->modelRangeStart.end(), std::make_move_iterator(data.modelRangeStart.begin()), std::make_move_iterator(data.modelRangeStart.end()));
    this->modelRangeEnd.insert(this->modelRangeEnd.end(), std::make_move_iterator(data.modelRangeEnd.begin()), std::make_move_iterator(data.modelRangeEnd.end()));
    this->isNonFullable.insert(this->isNonFullable.end(), std::make_move_iterator(data.isNonFullable.begin()), std::make_move_iterator(data.isNonFullable.end()));
    this->models.insert(this->models.end(), std::make_move_iterator(data.models.begin()), std::make_move_iterator(data.models.end()));
  }
};