#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in mat4 instanceModel;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} vs_out;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
  vs_out.vs_position = vec3(instanceModel * vec4(vertex_position, 1.0));
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1);
  
  mat3 normalMatrix = transpose(inverse(mat3(instanceModel)));
  vs_out.vs_normal = normalize(normalMatrix * vertex_normal);
  
  gl_Position = ProjectionMatrix * ViewMatrix * instanceModel * vec4(vertex_position, 1.f);
}