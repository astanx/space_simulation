#version 410

#include "ubo/camera.glsl"
#include "scale/scale.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 instancePosition;
layout (location = 3) in float instanceRadius;
layout (location = 4) in uint instanceTextureLayer;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  flat uint textureLayer;
} vs_out;

void main()
{
  vec3 right = vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
  vec3 up = vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

  vs_out.vs_texcoord = vertex_texcoord;

  vs_out.vs_position = instancePosition + right * vertex_position.x * instanceRadius + up * vertex_position.y * instanceRadius;

  vs_out.textureLayer = instanceTextureLayer;

  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vs_out.vs_position, 1.f);
}
