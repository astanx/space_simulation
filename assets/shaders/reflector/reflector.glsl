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

  if (NdotL <= 0.0 || NdotV <= 0.0)
    return 0.0;

  float i = acos(clamp(NdotL, -1.0, 1.0));
  float e = acos(clamp(NdotV, -1.0, 1.0));

  vec3 Lp = normalize(L - N * dot(L, N));
  vec3 Vp = normalize(V - N * dot(V, N));

  float cosPsi = clamp(dot(Lp, Vp), -1.0, 1.0);
  float psi = acos(cosPsi);

  float a = acos(clamp(dot(L, V), -1.0, 1.0));

  float mu_0 = effectiveNdotL(i, e, hapkeParameters.theta, psi);
  float mu = effectiveNdotV(i, e, hapkeParameters.theta, psi);

  float B = OppositionEffect(a, hapkeParameters.b0, hapkeParameters.h, hapkeParameters.b0_cb, hapkeParameters.h_cb);

  float P = ParticlePhase(a, hapkeParameters.b, hapkeParameters.c);

  float H1 = ChandrasekharMultipleScattering(mu_0, hapkeParameters.w);
  float H2 = ChandrasekharMultipleScattering(mu, hapkeParameters.w);

  float S = ShadowingTerm(i, e, hapkeParameters.theta, psi);

  return hapkeParameters.w / (4*PI) * mu_0 / (mu_0 + mu) * ( (1 + B) * P + H1*H2 - 1) * S;
}

#endif