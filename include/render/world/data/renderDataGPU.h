#pragma once

#include <glm/glm.hpp>

#include <vector>

struct RenderDataGPU
{
  std::vector<glm::vec3> instanceColors;
  std::vector<uint32_t> instanceTextureLayers;
  std::vector<float> instanceImportances;
  std::vector<uint32_t> modelRangeStart;
  std::vector<uint32_t> modelRangeEnd;

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
  }
};