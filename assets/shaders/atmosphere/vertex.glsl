#version 410

#include "ubo/camera.glsl"
#include "ubo/pbr_point_light.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} vs_out;

uniform mat4 ModelMatrix;

void main()
{
  vs_out.vs_position = vec3(ModelMatrix * vec4(vertex_position, 1.0));
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y);
  
  mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
  vs_out.vs_normal = normalize(normalMatrix * vertex_normal);

  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}