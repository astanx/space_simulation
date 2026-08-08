#ifndef FRUSTUM
#define FRUSTUM

#ifdef __OPENCL_VERSION__
typedef float3 vec3;
typedef float4 vec4;
#else
#include <glm/glm.hpp>
#include <resources/gpuTypes.h>
using vec3 = Vec3<float>;
using vec4 = glm::vec4;
using glm::dot;

enum FaceSide
{
  LEFT_FACE = 0,
  RIGHT_FACE,
  TOP_FACE,
  BOTTOM_FACE,
  NEAR_FACE,
  FAR_FACE
};
#endif

typedef struct Frustum
{
  vec4 faces[6];
} Frustum;

inline bool isVisibleSphere(const Frustum *frustum, vec3 pos, float radius)
{
  for (int face = 0; face < 6; face++)
  {
    vec4 plane = frustum->faces[face];
#ifdef __OPENCL_VERSION__
    float d = dot(plane.xyz, pos) + plane.w;
#else
    float d = dot(glm::vec3(plane.x, plane.y, plane.z), glm::vec3(pos.x, pos.y, pos.z)) + plane.w;
#endif
    if (d < -radius)
      return false;
  }
  return true;
}

inline bool shouldBeProcessed(const Frustum *frustum, vec3 pos, float radius)
{
  if (!frustum || !isVisibleSphere(frustum, pos, radius))
    return false;

  return true;
}

#endif
