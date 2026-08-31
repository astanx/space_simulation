#pragma once

#include "graphics/vertex.h"
#include "graphics/vertexAttribute.h"
#include "graphics/instanceStructs.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <array>
#include <cstddef>

enum class InstanceLayout
{
  PositionRadius,
  PositionRadiusTexture,
  PositionRadiusColor,
  ModelMatrix,
  ModelMatrixParts
};

inline const std::array<VertexAttribute, 2> POSITION_RADIUS = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadius, position)},
                                                                {1, 1, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadius, radius)}}};

inline const std::array<VertexAttribute, 3> POSITION_RADIUS_TEXTURE = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadiusTexture, position)},
                                                                        {1, 1, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadiusTexture, radius)},
                                                                        {2, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(InstancePositionRadiusTexture, textureLayer)}}};

inline const std::array<VertexAttribute, 3> POSITION_RADIUS_COLOR = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadiusColor, position)},
                                                                      {1, 1, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadiusColor, radius)},
                                                                      {2, 3, GL_FLOAT, GL_FALSE, offsetof(InstancePositionRadiusColor, color)}}};

inline const std::array<VertexAttribute, 4> MODEL_MATRIX = {{
    {4, 4, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrix, model)},
    {5, 4, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrix, model) + 1 * sizeof(glm::vec4)},
    {6, 4, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrix, model) + 2 * sizeof(glm::vec4)},
    {7, 4, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrix, model) + 3 * sizeof(glm::vec4)},
}};

inline const std::array<VertexAttribute, 3> MODEL_MATRIX_PARTS = {{
    {4, 3, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrixParts, position)},
    {5, 4, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrixParts, orientation)},
    {6, 3, GL_FLOAT, GL_FALSE, offsetof(InstanceModelMatrixParts, scale)},
}};

inline const std::map<InstanceLayout, LayoutDesc> INSTANCE_LAYOUTS = {
    {InstanceLayout::PositionRadius, {POSITION_RADIUS.data(), POSITION_RADIUS.size()}},
    {InstanceLayout::PositionRadiusTexture, {POSITION_RADIUS_TEXTURE.data(), POSITION_RADIUS_TEXTURE.size()}},
    {InstanceLayout::PositionRadiusColor, {POSITION_RADIUS_COLOR.data(), POSITION_RADIUS_COLOR.size()}},
    {InstanceLayout::ModelMatrix, {MODEL_MATRIX.data(), MODEL_MATRIX.size(), false}},
    {InstanceLayout::ModelMatrixParts, {MODEL_MATRIX_PARTS.data(), MODEL_MATRIX_PARTS.size(), false}},
};