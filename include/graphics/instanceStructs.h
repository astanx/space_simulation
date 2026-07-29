#ifndef INSTANCE_STRUCTS_H
#define INSTANCE_STRUCTS_H

#ifdef __OPENCL_VERSION__
typedef float3 vec3;
typedef float4 quat;
typedef uint uint32_t;
typedef struct
{
  float4 cols[4];
} mat4;
#else
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
using vec3 = glm::vec3;
using quat = glm::quat;
using uint = std::uint32_t;
using mat4 = glm::mat4;
#endif

typedef struct InstanceData
{
  vec3 position;
  vec3 color;
  uint textureLayer;
  float radius;
} InstanceData;

typedef struct InstancePositionRadius
{
  vec3 position;
  float radius;
} InstancePositionRadius;

typedef struct InstancePositionRadiusTexture
{
  vec3 position;
  float radius;
  uint textureLayer;
} InstancePositionRadiusTexture;

typedef struct InstancePositionRadiusColor
{
  vec3 position;
  float radius;
  vec3 color;
} InstancePositionRadiusColor;

typedef struct InstanceModelMatrix
{
  mat4 model;
} InstanceModelMatrix;

typedef struct InstanceModelMatrixParts
{
  vec3 position;
  quat orientation;
  vec3 scale;
} InstanceModelMatrixParts;

#endif