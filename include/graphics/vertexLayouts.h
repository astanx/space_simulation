#pragma once

#include "graphics/vertex.h"
#include "graphics/vertexAttribute.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>

enum class VertexLayout
{
  Empty,
  Full,
  NoColor,
  PositionOnly,
  PositionTexcoord,
  PositionNormalTangent
};

constexpr std::array<VertexAttribute, 0> EMPTY = {};

constexpr std::array<VertexAttribute, 4> FULL = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)},
                                                  {1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord)},
                                                  {2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal)},
                                                  {3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color)}}};

constexpr std::array<VertexAttribute, 3> NO_COLOR = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)},
                                                      {1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord)},
                                                      {2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal)}}};

constexpr std::array<VertexAttribute, 1> POSITION_ONLY = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)}}};

constexpr std::array<VertexAttribute, 2> POSITION_TEXCOORD = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)},
                                                               {1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord)}}};

constexpr std::array<VertexAttribute, 4> POSITION_NORMAL_TANGENT = {{{0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)},
                                                                     {1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord)},
                                                                     {2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal)},
                                                                     {3, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent)}}};

inline const std::map<VertexLayout, LayoutDesc> LAYOUTS = {
    {VertexLayout::Empty, {EMPTY.data(), EMPTY.size()}},
    {VertexLayout::Full, {FULL.data(), FULL.size()}},
    {VertexLayout::NoColor, {NO_COLOR.data(), NO_COLOR.size()}},
    {VertexLayout::PositionOnly, {POSITION_ONLY.data(), POSITION_ONLY.size()}},
    {VertexLayout::PositionTexcoord, {POSITION_TEXCOORD.data(), POSITION_TEXCOORD.size()}},
    {VertexLayout::PositionNormalTangent, {POSITION_NORMAL_TANGENT.data(), POSITION_NORMAL_TANGENT.size()}}};