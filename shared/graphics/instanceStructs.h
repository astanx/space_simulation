#ifndef INSTANCE_STRUCTS_H
#define INSTANCE_STRUCTS_H

#ifdef __OPENCL_VERSION__
#include "matrix.cl"
#include "quaternion.cl"
typedef float3 vec3;
typedef uint uint32_t;
#else
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <resources/gpuTypes.h>
using vec3 = Vec3<float>; // has padding, so vec4 alignment
using quat = Quat<float>;
using uint = std::uint32_t;
using mat4 = glm::mat4;
#endif

typedef struct InstanceData
{
  vec3 position;
  vec3 color;
  uint textureLayer;
  float radius;
  float _pad0;
  float _pad1;
} InstanceData;

typedef struct InstancePositionRadius
{
  vec3 position;
  float radius;
  float _pad0;
  float _pad1;
  float _pad2;
} InstancePositionRadius;

typedef struct InstancePositionRadiusTexture
{
  vec3 position;
  float radius;
  uint textureLayer;
  uint _pad0;
  uint _pad1;
  uint _pad2;
  uint _pad3;
  uint _pad4;
  uint _pad5;
} InstancePositionRadiusTexture;

typedef struct InstancePositionRadiusColor
{
  vec3 position;
  vec3 color;
  float radius;
  float _pad0;
  float _pad1;
  float _pad2;
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