#ifndef SHADOW_UBO_GLSL
#define SHADOW_UBO_GLSL

layout (std140) uniform PointShadowBuffer
{
  //PointShadow pointShadow;
  mat4 ShadowMatrices[6];
  vec4 lightPos;
  float far_plane;
  vec3 _pad0;
};

#endif