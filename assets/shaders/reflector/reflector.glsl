#ifndef REFLECTOR_GLSL
#define REFLECTOR_GLSL

#include "reflector/reflector_helper.glsl"

struct HapkeParameters
{
  float w; // single scattering albedo
  float theta; // macroscopic roughness angle (radians)
  float h; // opposition effect width
  float b0; // strength of the opposition effect  
  float h_cb; // width of the coherent backscatter opposition effect
  float b0_cb; // strength of the coherent backscatter opposition effect
  float b; // asymmetry parameter
  float c; // weighting between backward and forward scattering
};

float hapkeBRDF(vec3 N, vec3 V, vec3 L, HapkeParameters hapkeParameters)
{
  float NdotL = max(dot(N, L), 0.0);
  float NdotV = max(dot(N, V), 0.0);

  float i = acos(clamp(NdotL, -1.0, 1.0));
  float e = acos(clamp(NdotV, -1.0, 1.0));

  vec3 Lp = L - N * dot(L, N);
  vec3 Vp = V - N * dot(V, N);

  float lenLp = length(Lp);
  float lenVp = length(Vp);

  float cosPsi = 1.0; 

  if (lenLp > 1e-5 && lenVp > 1e-5) 
    cosPsi = dot(Lp / lenLp, Vp / lenVp);

  cosPsi = clamp(cosPsi, -1.0, 1.0);
  float psi = acos(cosPsi);

  float cos_a = clamp(dot(L, V), -1.0, 1.0);

  float mu_0 = effectiveNdotL(i, e, hapkeParameters.theta, psi);
  float mu = effectiveNdotV(i, e, hapkeParameters.theta, psi);

  if (mu_0 <= 0.0 || mu <= 0.0)
    return 0.0;

  float B = OppositionEffect(cos_a, hapkeParameters.b0, hapkeParameters.h, hapkeParameters.b0_cb, hapkeParameters.h_cb);

  float P = ParticlePhase(cos_a, hapkeParameters.b, hapkeParameters.c);

  float H1 = H(mu_0, hapkeParameters.w);
  float H2 = H(mu, hapkeParameters.w);

  float M = H1*H2 - 1;

  float S = ShadowingTerm(i, e, hapkeParameters.theta, psi);

  return hapkeParameters.w / (4*PI) * mu_0 / (mu_0 + mu) * ( (1 + B + M) * P) * S;
}

#endif