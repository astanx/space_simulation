#ifndef POINTSHADOW_GLSL
#define POINTSHADOW_GLSL


/*
struct PointShadow
{
  vec3 lightPos;
  float far_plane;
  float near_plane;
  float aspect_ratio;
  int enabled;

  float _pad0;
};

struct Vec3Pair
{
  vec3 a;
  vec3 b;
};

const Vec3Pair MATRICES[6] = Vec3Pair[6](
  Vec3Pair(vec3( 1.0,  0.0,  0.0), vec3( 0.0, -1.0,  0.0)),
  Vec3Pair(vec3(-1.0,  0.0,  0.0), vec3( 0.0, -1.0,  0.0)),
  Vec3Pair(vec3( 0.0,  1.0,  0.0), vec3( 0.0,  0.0,  1.0)),
  Vec3Pair(vec3( 0.0, -1.0,  0.0), vec3( 0.0,  0.0, -1.0)),
  Vec3Pair(vec3( 0.0,  0.0,  1.0), vec3( 0.0, -1.0,  0.0)),
  Vec3Pair(vec3( 0.0,  0.0, -1.0), vec3( 0.0, -1.0,  0.0))
)

mat4 CalcProjMatrix(float aspect, float nearPlane, float farPlane)
{
  float fov = radians(90.0);
  float f = 1.0 / tan(fov * 0.5);

  return mat4(
    f / aspect, 0.0, 0.0,                                                   0.0,
    0.0,        f,   0.0,                                                   0.0,
    0.0,        0.0, (farPlane + nearPlane) / (nearPlane - farPlane),      -1.0,
    0.0,        0.0, (2.0 * farPlane * nearPlane) / (nearPlane - farPlane), 0.0
  );
}

mat4 calcLookAtMatrix(vec3 eye, vec3 right, vec3 up)
{
  vec3 center = eye + right;

  vec3 F = normalize(center - eye);
  vec3 S = normalize(cross(F, up));
  vec3 U = cross(S, F);

  return mat4(
    vec4(S, -dot(S, eye)),
    vec4(U, -dot(U, eye)),
    vec4(-F, dot(F, eye)),
    vec4(0.0, 0.0, 0.0, 1.0)
  );
}

void calcShadowMatrices(out mat4 shadowMatrices[6], in mat4 shadowProj[6], in mat4 lookAt[6]) {
  for (int i = 0; i < 6; i++) {
    shadowMatrices[i] = shadowProj[i] * lookAt[i];
  }
}
*/

float CalcPointShadow(vec3 pos, vec4 lightPos, samplerCube depthMap, float far_plane, vec3 normal)
{
  vec3 fragToLight = pos - lightPos.xyz;
  float currentDepth = length(fragToLight);
  if (currentDepth < 0.0001) return 0.0;
  vec3 sampleDir = normalize(fragToLight);
  float closestDepth = texture(depthMap, sampleDir).r;
  closestDepth *= far_plane;
  vec3 lightDir = normalize(lightPos.xyz - pos);
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  float shadow = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
  return shadow;
}


#endif