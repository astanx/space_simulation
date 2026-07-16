#version 410

#include "modelMatrix/modelMatrix.glsl"

#include "ubo/camera.glsl"
#include "ubo/pbr_point_light.glsl"

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
#ifdef TANGENT
layout (location = 3) in vec4 vertex_tangent;
#endif
//layout (location = 4) in mat4 ModelMatrix;
layout (location = 4) in vec3 instancePosition;
layout (location = 5) in vec4 instanceOrientation;
layout (location = 6) in vec3 instanceScale;


out VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;

  #ifdef TANGENT
  mat3 TBN;
  #endif
} vs_out;

void main()
{
  //vs_out.vs_position = vec3(ModelMatrix * vec4(vertex_position, 1.0));
  vs_out.vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y);
  
  //mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
  //vs_out.vs_normal = normalize(normalMatrix * vertex_normal);
  
  vec3 p;
  vec3 N;

  #ifdef TANGENT
    vec3 T;

    transform(instancePosition, instanceOrientation, instanceScale, vertex_position, vertex_normal, vertex_tangent.xyz, p, N, T);

    // Gram-Schmidt re-orthogonalization
    T = normalize(T - dot(T, N) * N);

    vec3 B = normalize(cross(N, T)) * vertex_tangent.w;

    vs_out.TBN = mat3(T, B, N);
  #else 
    transform(instancePosition, instanceOrientation, instanceScale, vertex_position, vertex_normal, p, N);
  #endif

  vs_out.vs_position = p;
  vs_out.vs_normal = N;

  gl_Position = ProjectionMatrix * ViewMatrix * vec4(p, 1.f);
}