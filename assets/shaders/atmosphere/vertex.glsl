#version 410

#include "modelMatrix/modelMatrix.glsl"

#include "ubo/camera.glsl"
#include "ubo/pbr_point_light.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 4) in vec3 instancePosition;
layout (location = 5) in vec4 instanceOrientation;
layout (location = 6) in vec3 instanceScale;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} vs_out;

void main()
{
  vec3 p;
  vec3 n;
  transform(instancePosition, instanceOrientation, instanceScale, p, n);
  vs_out.vs_position = p;
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y);
  
  vs_out.vs_normal = n;

  gl_Position = ProjectionMatrix * ViewMatrix * vec4(p, 1.f);
}