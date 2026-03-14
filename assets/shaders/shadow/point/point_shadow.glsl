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

vec3 gridSamplingDisk[20] = vec3[](
  vec3(0.5381, 0.1856, -0.4319),
  vec3(0.1379, 0.2486, 0.4430),
  vec3(0.3371, 0.5679, -0.0057),
  vec3(-0.6999, -0.0451, -0.0019),
  vec3(0.0689, -0.1598, -0.8547),
  vec3(0.0560, 0.0069, -0.1843),
  vec3(-0.0146, 0.1402, 0.0762),
  vec3(0.0100, -0.1924, -0.0344),
  vec3(-0.3577, -0.5301, -0.4358),
  vec3(-0.3169, 0.1063, 0.0158),
  vec3(0.0103, -0.5869, 0.0046),
  vec3(-0.0897, -0.4940, 0.3287),
  vec3(0.7119, -0.0154, -0.0918),
  vec3(-0.0533, 0.0596, -0.5411),
  vec3(0.0352, -0.0631, 0.5460),
  vec3(-0.4776, 0.2847, -0.0271),
  vec3(-0.0795, 0.3185, 0.2123),
  vec3(0.2924, -0.3127, -0.1760),
  vec3(-0.1400, -0.1346, 0.2555),
  vec3(-0.0550, -0.0627, -0.3161)
);


float CalcPointShadow(vec3 pos, vec4 lightPos, samplerCube depthMap, float far_plane, vec3 normal)
{
  vec3 fragToLight = pos - lightPos.xyz;
  float currentDepth = length(fragToLight);

  if (currentDepth < 0.0001)
    return 0.0;

  vec3 lightDir = normalize(lightPos.xyz - pos);

  // backface optimization
  if (dot(normal, lightDir) <= 0.0)
    return 0.0;

  float bias = max(0.003 * (1.0 - dot(normal, lightDir)), 0.0005);

  // distance-based filter radius
  float viewDistance = currentDepth / far_plane;
  float diskRadius = mix(0.01, 0.12, viewDistance);

  float shadow = 0.0;
  int samples = 20;

  for (int i = 0; i < samples; ++i)
  {
    vec3 sampleDir = normalize(fragToLight + gridSamplingDisk[i] * diskRadius);
    float closestDepth = texture(depthMap, sampleDir).r;
    closestDepth *= far_plane;

    if (currentDepth - bias > closestDepth)
      shadow += 1.0;
  }

  shadow /= float(samples);

  return shadow;
}


#endif