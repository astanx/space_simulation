#ifndef CAMERA_UBO_GLSL
#define CAMERA_UBO_GLSL

layout (std140) uniform Camera
{
  mat4 ProjectionMatrix;
  mat4 ViewMatrix;
  vec4 camPosition;
};

#endif