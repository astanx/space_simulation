#version 410

#include "ubo/camera.glsl"
#include "ubo/pbr_point_light.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec4 vertex_tangent;
layout (location = 4) in vec3 instancePosition;

out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  vec3 vs_tangentLightPos;
  vec3 vs_tangentCamPos;
  vec3 vs_tangentPos;
} vs_out;

void main()
{
  vs_out.vs_position = vertex_position + instancePosition;
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1);
  
  vs_out.vs_normal = normalize(vertex_normal);

  vec3 N = vs_out.vs_normal;
  vec3 T = normalize(vertex_tangent.xyz);

  // Gram-Schmidt re-orthogonalization
  //T = normalize(T - dot(T, N) * N);

  vec3 B = cross(N, T) * vertex_tangent.w;

  mat3 TBN = transpose(mat3(T, B, N));

  vs_out.vs_tangentLightPos = TBN * pbrPointLight.position;
  vs_out.vs_tangentCamPos  = TBN * camPosition.xyz;
  vs_out.vs_tangentPos  = TBN * vs_out.vs_position;
  
  gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertex_position + instancePosition, 1.f);
}