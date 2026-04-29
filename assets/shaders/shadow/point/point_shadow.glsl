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

const vec3 poissonDisk[32] = vec3[](
    vec3(-0.94201624, -0.39906216,  0.00000000),
    vec3( 0.94558609, -0.76890725,  0.00000000),
    vec3(-0.09418410, -0.92938870,  0.00000000),
    vec3( 0.34495938,  0.29387760,  0.00000000),
    vec3(-0.91588581,  0.45771432,  0.00000000),
    vec3(-0.81544232, -0.87912464,  0.00000000),
    vec3(-0.38277543,  0.27676845,  0.00000000),
    vec3( 0.97484398,  0.75648379,  0.00000000),
    vec3( 0.44323325, -0.97511554,  0.00000000),
    vec3( 0.53742981, -0.47373420,  0.00000000),
    vec3(-0.26496911, -0.41893023,  0.00000000),
    vec3( 0.79197514,  0.19090188,  0.00000000),
    vec3(-0.24188840,  0.99706507,  0.00000000),
    vec3(-0.81409955,  0.91437590,  0.00000000),
    vec3( 0.19984126,  0.78641367,  0.00000000),
    vec3( 0.14383161, -0.14100713,  0.00000000),
    vec3(-0.10107862, -0.83720547,  0.00000000),
    vec3(-0.68820405,  0.18308620,  0.00000000),
    vec3( 0.56770492, -0.60564148,  0.00000000),
    vec3( 0.06332600,  0.14236932,  0.00000000),
    vec3( 0.32430196, -0.01524240,  0.00000000),
    vec3(-0.78014514, -0.48625138,  0.00000000),
    vec3(-0.37186815,  0.88213818,  0.00000000),
    vec3( 0.20047664,  0.49443019,  0.00000000),
    vec3(-0.49455280,  0.06961672,  0.00000000),
    vec3( 0.60561586,  0.19282341,  0.00000000),
    vec3(-0.24124980, -0.07487576,  0.00000000),
    vec3(-0.39065485, -0.03535364,  0.00000000),
    vec3(-0.54979134, -0.28775349,  0.00000000),
    vec3( 0.84791017, -0.21286947,  0.00000000),
    vec3(-0.05099079, -0.42224942,  0.00000000),
    vec3(-0.65938362,  0.61538874,  0.00000000)
);

float CalcPointShadowPCF(vec3 fragToLight, float currentDepth, samplerCube depthMap, float far_plane, vec3 normal)
{
  float shadow = 0.0;
  const int samples = 32;

  float viewDistance = currentDepth / far_plane;
  float diskRadius   = mix(0.22, 3.1, viewDistance);

  float bias = 0.0025 + 0.038 * (1.0 - dot(normal, normalize(fragToLight)));

  for (int i = 0; i < samples; ++i)
  {
    float angle = fract(sin(dot(gl_FragCoord.xy + float(i)*vec2(17.3, 59.1), vec2(12.9898, 78.233))) * 43758.5453) * 6.2831853;

    vec3 offset = poissonDisk[i] * diskRadius;
    float s = sin(angle);
    float c = cos(angle);
    vec2 rotated = mat2(c, -s, s, c) * offset.xy;
    offset = vec3(rotated, offset.z * 0.7);

    vec3 sampleDir = normalize(fragToLight + offset);
    float closestDepth = texture(depthMap, sampleDir).r * far_plane;

    if (currentDepth - bias > closestDepth)
      shadow += 1.0;
  }
  shadow /= float(samples);
  return 1.0 - shadow;
}

float CalcPointShadow(vec3 pos, vec4 lightPos, samplerCube depthMap, samplerCube esmMap, float far_plane, vec3 normal)
{
  vec3 fragToLight = pos - lightPos.xyz;
  float currentDepth = length(fragToLight);

  if (currentDepth > far_plane)
    return 1.0;

  float c = 40.0;

  vec3 dir = normalize(fragToLight);

  float filtered = texture(esmMap, dir).r;

  float currentDepthNorm = currentDepth / far_plane;

  float esm = filtered * exp(-c * currentDepthNorm);

  if (isnan(esm) || isinf(esm) || esm > 1.5f || esm < -0.2f || true) // force pcf anyway, esm is not stable
    return CalcPointShadowPCF(fragToLight, currentDepth, depthMap, far_plane, normal);
  
  return clamp(esm, 0.0, 1.0);
}

#endif