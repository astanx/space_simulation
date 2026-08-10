#pragma once

#include "resources/gpuTypes.h"

#include <glm/glm.hpp>

#include <vector>

class Model;

struct RenderDataGPU
{
  std::vector<Vec3<float>> modelColors;
  std::vector<uint32_t> modelTextureLayers;
  std::vector<float> modelImportances;
  std::vector<uint32_t> modelRangeStart;
  std::vector<uint32_t> modelRangeEnd;
  std::vector<uint32_t> isNonFullable;
  std::vector<Model *> models;

  void resize(size_t n)
  {
  }

  void combine(RenderDataGPU &data)
  {
    this->modelColors.insert(this->modelColors.end(), std::make_move_iterator(data.modelColors.begin()), std::make_move_iterator(data.modelColors.end()));
    this->modelTextureLayers.insert(this->modelTextureLayers.end(), std::make_move_iterator(data.modelTextureLayers.begin()), std::make_move_iterator(data.modelTextureLayers.end()));
    this->modelImportances.insert(this->modelImportances.end(), std::make_move_iterator(data.modelImportances.begin()), std::make_move_iterator(data.modelImportances.end()));
    this->modelRangeStart.insert(this->modelRangeStart.end(), std::make_move_iterator(data.modelRangeStart.begin()), std::make_move_iterator(data.modelRangeStart.end()));
    this->modelRangeEnd.insert(this->modelRangeEnd.end(), std::make_move_iterator(data.modelRangeEnd.begin()), std::make_move_iterator(data.modelRangeEnd.end()));
    this->isNonFullable.insert(this->isNonFullable.end(), std::make_move_iterator(data.isNonFullable.begin()), std::make_move_iterator(data.isNonFullable.end()));
    this->models.insert(this->models.end(), std::make_move_iterator(data.models.begin()), std::make_move_iterator(data.models.end()));
  }
};