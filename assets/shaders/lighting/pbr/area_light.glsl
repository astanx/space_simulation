#ifndef AREA_LIGHT_GLSL
#define AREA_LIGHT_GLSL

#include "lighting/pbr/pbr_point_light.glsl"
#include "lighting/pbr/point_light_helper.glsl"
#include "material/pbrMaterial.glsl"
#include "constants/constants.glsl"

vec3 integrateEdgeVec(vec3 v1, vec3 v2) {
  float x = dot(v1, v2);
  float y = abs(x);
  float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
  float b = 3.4175940 + (4.1616724 + y)*y;
  float v = a / b;
  float sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;
  return cross(v1, v2) * sintheta;
}

void sphere_evaluate(vec3 C, float R, vec3 P, vec3 r, float n, out vec3 points[4])
{
  float m = sqrt(2 / (n + 2));
  
  vec3 p = (P - C) / R;

  vec3 nD = normalize(p);
  float d = length(p);

  float denom = max(sqrt(d * d - 1.0), 1e-5);
  float s = d / denom;

  float k = abs(dot(nD, p)) / d;
  s *= 1 - m * k + m * k * sqrt(PI/2);

  float denomR = max(dot(nD, r), 1e-5);
  float t = -dot(nD, p) / denomR;

  vec3 r_ = p + t * r;
  vec3 p_ = p - dot(nD, p) * nD;
  
  vec3 v0 = s * normalize((m - 1) * r_ + m * p_);
  vec3 v1 = normalize(cross(v0, nD));
  vec3 v2 = v0 * -1.0;
  vec3 v3 = v1 * -1.0;

  points[0] = C + v0 * R;
  points[1] = C + v1 * R;
  points[2] = C + v2 * R;
  points[3] = C + v3 * R; 
}

vec3 LTC_specular(vec3 N, vec3 V, vec3 P, PBRPointLight light, float roughness) 
{
  vec3 r = reflect(-V, N);
  r = normalize(r);

  float n = 2 / (roughness * roughness) - 2;

  vec3 v[4];
  sphere_evaluate(light.position, light.radius, P, r, n, v);
  
  for (int i = 0; i < 4; i++)
    v[i] = normalize(v[i] - P);

  float M = 0.0;
  for(int i = 0; i < 4; i++)
  {
    vec3 v1 = v[i];
    vec3 v2 = v[(i+1)%4];

    float c1 = dot(v1, r);
    float c2 = dot(v2, r);

    bool above1 = c1 > 0.0;
    bool above2 = c2 > 0.0;

    if(!above1 && !above2)
      continue;

    if(above1 != above2)
    {
      float t = c1 / (c1 - c2);
      vec3 vh = normalize(mix(v1, v2, t));

      if(!above1)
        v1 = vh;
      else
        v2 = vh;

      c1 = dot(v1, r);
      c2 = dot(v2, r);
    }

    vec3 crossEdge = cross(v1, v2);
    float dotCrossR = dot(crossEdge, r);

    float root = sqrt(max(0.0, (1.0 - c1*c1) * (1.0 - c2*c2)));

    float denom = c1*c2 + root;

    float weight = (c1 + c2) / ((n + 1.0) * (1.0 + denom));

    float I_edge = (1.0 / (2.0 * PI)) * weight * atan(dotCrossR, denom);

    M += I_edge;
  }
  M = max(M, 0.0);

  return vec3(M);
}

vec3 LTC_diffuse(vec3 N, vec3 V, vec3 P, PBRPointLight light, vec3 albedo)
{
  /*
  float d = length(light.position - P);

  return albedo * light.radius * light.radius / (d * d) * max(0.0, dot(N, normalize(light.position - P)));
  */

  vec3 L  = light.position - P;
  float dist = length(L);

  vec3 Ld = L / dist;

  float sinTheta = clamp(light.radius / dist, 0.0, 1.0);
  float sinTheta2 = sinTheta * sinTheta;

  float omega = 2.0 * PI * (1.0 - sqrt(1.0 - sinTheta2));

  float NdotL = max(dot(N, Ld), 0.0);

  return albedo * omega * NdotL / PI;
}

/*
uniform sampler2D LTC1;
uniform sampler2D LTC2;

const int POINTS = 512;

const float LUT_SIZE = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS = 0.5/LUT_SIZE;

void sphere_evaluate(vec3 C, float R, vec3 P, out vec3 points[POINTS])
{
  vec3 L = normalize(C - P);

  float sinTheta = clamp(R / length(C - P), 0.0, 1.0);
  float cosTheta = sqrt(1.0 - sinTheta * sinTheta);

  vec3 T1 = normalize(cross(abs(L.x) < 0.9 ? vec3(1,0,0) : vec3(0,1,0), L));
  vec3 T2 = cross(L, T1);

  for (int i = 0; i < POINTS; i++)
  {
    float angle = 2.0 * PI * float(i) / float(POINTS);
    points[i] = C + R * (cosTheta * L + sinTheta * (cos(angle) * T1 + sin(angle) * T2));
  }
}

vec3 LTC_evaluate(vec3 N, vec3 V, vec3 P, PBRPointLight light, mat3 Minv) 
{
  // construct orthonormal basis around N
  vec3 up = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
  vec3 T1 = normalize(cross(up, N));
  vec3 T2 = cross(N, T1);

  // rotate area light in (T1, T2, N) basis
  Minv = Minv * transpose(mat3(T1, T2, N));

  vec3 L[POINTS];
  vec3 points[POINTS];
  sphere_evaluate(light.position, light.radius, P, points);

  for (int i = 0; i < POINTS; i++)
    L[i] = normalize(Minv * (points[i] - P));

  vec3 vsum = vec3(0.0);
  for (int i = 0; i < POINTS; i++)
  {
    if (i == POINTS - 1)
      vsum += integrateEdgeVec(L[i], L[0]);
    else
      vsum += integrateEdgeVec(L[i], L[i + 1]);
  }

  // form factor of the polygon in direction vsum
  float len = length(vsum);

  float invLen = 1.0 / max(len, 1e-5);
  float z = vsum.z * invLen;

  vec2 uvClip = vec2(z*0.5f + 0.5f, len); // range [0, 1]
  uvClip = uvClip*LUT_SCALE + LUT_BIAS;

  // Fetch the form factor for horizon clipping
  float scale = texture(LTC2, uvClip).w;

  float sum = len*scale;

  return vec3(sum);
}

vec3 LTC_diffuse(vec3 N, vec3 V, vec3 P, PBRPointLight light, vec3 albedo)
{
  mat3 Minv = mat3(1.0);
  vec3 E = LTC_evaluate(N, V, P, light, Minv);

  return albedo / PI * E;
}

vec3 LTC_specular(vec3 N, vec3 V, vec3 P, PBRPointLight light, MaterialData material, vec3 F0) {
  float dotNV = clamp(dot(N, V), 0.0f, 1.0f);

  // use roughness and sqrt(1-cos_theta) to sample M_texture
  vec2 uv = vec2(material.roughness, sqrt(1.0f - dotNV));
  uv = uv*LUT_SCALE + LUT_BIAS;

  // get 4 parameters for inverse_M
  vec4 t1 = texture(LTC1, uv);

  // Get 2 parameters for Fresnel calculation
  vec4 t2 = texture(LTC2, uv);

  mat3 Minv = mat3(
    vec3(t1.x, 0, t1.y),
    vec3(   0, 1,    0),
    vec3(t1.z, 0, t1.w)
  );

  vec3 E = LTC_evaluate(N, V, P, light, Minv);

  vec3 specular = (F0 * t2.x + (1.0 - F0) * t2.y) * E;

  return specular;
}
*/


#endif