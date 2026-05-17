#ifndef AREA_LIGHT_GLSL
#define AREA_LIGHT_GLSL

#include "lighting/pbr/pbr_point_light.glsl"
#include "lighting/pbr/point_light_helper.glsl"
#include "material/pbrMaterial.glsl"
#include "constants/constants.glsl"

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

vec3 specular_evaluate(vec3 N, vec3 V, vec3 P, PBRPointLight light, float roughness) 
{
  vec3 r = reflect(-V, N);
  r = normalize(r);

  roughness = max(roughness, 0.045);

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

vec3 diffuse_evaluate(vec3 N, vec3 V, vec3 P, PBRPointLight light, vec3 albedo)
{
  vec3 L  = light.position - P;
  float dist = length(L);

  vec3 Ld = L / dist;

  float sinTheta = clamp(light.radius / dist, 0.0, 1.0);
  float sinTheta2 = sinTheta * sinTheta;

  float omega = 2.0 * PI * (1.0 - sqrt(1.0 - sinTheta2));

  float NdotL = max(dot(N, Ld), 0.0);

  return albedo * omega * NdotL / PI;
}

#endif