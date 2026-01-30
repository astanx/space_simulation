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

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


float CalcPointShadow(vec3 pos, vec4 lightPos, samplerCube depthMap, float far_plane, vec3 normal)
{
  vec3 fragToLight = pos - lightPos.xyz;
  float currentDepth = length(fragToLight);
  if (currentDepth < 0.0001) return 0.0;
  vec3 sampleDir = normalize(fragToLight);
  float closestDepth = texture(depthMap, sampleDir).r;
  closestDepth *= far_plane;
  vec3 lightDir = normalize(lightPos.xyz - pos);


    // PCF
  float shadow = 0.0;
  float bias = 
    0.0008 * (1.0 + currentDepth / far_plane) + 
    0.003 * (1.0 - dot(normal, lightDir));

  /*float samples = 4.0;
  float offset = 0.1;
  for(float x = -offset; x < offset; x += offset / (samples * 0.5))
  {
    for(float y = -offset; y < offset; y += offset / (samples * 0.5))
    {
      for(float z = -offset; z < offset; z += offset / (samples * 0.5))
      {
        float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
        closestDepth *= far_plane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
          shadow += 1.0;
      }
    }
  }
  shadow /= (samples * samples * samples);
  */

  int samples = 20;
  float diskRadius = mix(0.01, 0.12, currentDepth / far_plane);

  for (int i = 0; i < samples; ++i)
  {
    vec3 sampleDir = normalize(fragToLight + gridSamplingDisk[i] * diskRadius);
    float closestDepth = texture(depthMap, sampleDir).r * far_plane;
    if (currentDepth - bias > closestDepth)
      shadow += 1.0;
  }
  shadow /= float(samples);


  return shadow;
}


#endif