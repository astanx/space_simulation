#version 410

#include "ubo/camera.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec3 vertex_color;

out VS_OUT {
  vec3 vs_position;
  vec3 vs_color;
  vec2 vs_texcoord;
  vec3 vs_normal;
} vs_out;

uniform mat4 ModelMatrix;
uniform bool useModelMatrix;

mat4 getModelMatrix()
{
  return useModelMatrix ?
  ModelMatrix 
  : mat4(1.0);
}

void main()
{
  mat4 model = getModelMatrix();
  vs_out.vs_position = vec3(model * vec4(vertex_position, 1.0));
  vs_out.vs_color = vertex_color;
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1);
  
  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vs_out.vs_normal = normalize(normalMatrix * vertex_normal);

  gl_Position = ProjectionMatrix * ViewMatrix * model * vec4(vertex_position, 1.f);
}