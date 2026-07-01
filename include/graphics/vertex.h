#pragma once

#include <glm/glm.hpp>

struct VertexPositionTexcoordNormalTangent
{
  glm::vec3 position;
  glm::vec2 texcoord;
  glm::vec3 normal;
  glm::vec4 tangent;

  void init(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec2 &texcoord = glm::vec2(0.0f), const glm::vec3 &normal = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(0.0f), const glm::vec4 &tangent = glm::vec4(0.0f))
  {
    this->position = position;
    this->texcoord = texcoord;
    this->normal = normal;
    this->tangent = tangent;
  }
};

struct VertexPositionTexcoordNormalColor
{
  glm::vec3 position;
  glm::vec2 texcoord;
  glm::vec3 normal;
  glm::vec3 color;

  void init(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec2 &texcoord = glm::vec2(0.0f), const glm::vec3 &normal = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(0.0f), const glm::vec4 &tangent = glm::vec4(0.0f))
  {
    this->position = position;
    this->texcoord = texcoord;
    this->normal = normal;
    this->color = color;
  }
};

struct VertexPositionTexcoordNormal
{
  glm::vec3 position;
  glm::vec2 texcoord;
  glm::vec3 normal;

  void init(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec2 &texcoord = glm::vec2(0.0f), const glm::vec3 &normal = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(0.0f), const glm::vec4 &tangent = glm::vec4(0.0f))
  {
    this->position = position;
    this->texcoord = texcoord;
    this->normal = normal;
  }
};

struct VertexPosition
{
  glm::vec3 position;

  void init(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec2 &texcoord = glm::vec2(0.0f), const glm::vec3 &normal = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(0.0f), const glm::vec4 &tangent = glm::vec4(0.0f))
  {
    this->position = position;
  }
};

struct VertexPositionTexcoord
{
  glm::vec3 position;
  glm::vec2 texcoord;

  void init(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec2 &texcoord = glm::vec2(0.0f), const glm::vec3 &normal = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(0.0f), const glm::vec4 &tangent = glm::vec4(0.0f))
  {
    this->position = position;
    this->texcoord = texcoord;
  }
};

struct VertexEmpty
{
  void init(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec2 &texcoord = glm::vec2(0.0f), const glm::vec3 &normal = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(0.0f), const glm::vec4 &tangent = glm::vec4(0.0f))
  {
  }
};