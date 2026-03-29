#ifndef REFLECTOR_HELPER_GLSL
#define REFLECTOR_HELPER_GLSL

#include "constants/constants.glsl"

float cot(float x)
{
  return cos(x) / max(sin(x), 1e-6);
}

float E1(float theta, float a)
{
  return exp(-(2* cot(theta)) / PI * cot(a));
}

float E2(float theta, float a)
{
  return exp(-(pow(cot(theta), 2)) / PI * pow(cot(a), 2));
}

float X(float theta)
{
  return 1 / sqrt(1 + PI * pow(tan(theta), 2));
}

float effectiveNdotL(float i, float e, float theta, float psi)
{
  float NdotL = cos(i);
  float NdotV = cos(e);

  float effectiveNdotL = X(theta) * (NdotL - sqrt(1 - pow(NdotL, 2)) * tan(theta) * 
    (E2(theta, e) - pow(sin(psi/2), 2) * E2(theta, i)) 
    / (E2(theta, e) - E1(theta, e) * (psi/PI)));

  return effectiveNdotL;
}

float effectiveNdotV(float i, float e, float theta, float psi)
{
  float NdotL = cos(i);
  float NdotV = cos(e);

  float effectiveNdotV = X(theta) * (NdotV - sqrt(1 - pow(NdotV, 2)) * tan(theta) * E2(theta, e));

  return effectiveNdotV;
}

float ShadowingTerm(float i, float e, float theta, float psi)
{
  float n_i = sqrt(1 - pow((sin(i) * tan(theta)), 2));
  float n_e = sqrt(1 - pow((sin(e) * tan(theta)), 2));

  float x = X(theta);
  float f = psi/PI;

  float mu_0 = effectiveNdotL(i, e, theta, psi);
  float mu = effectiveNdotV(i, e, theta, psi);

  float numerator = mu_0 / n_i * mu / n_e * x;
  float denominator = 1 - f + f * x * mu_0/n_i;

  return numerator / denominator;
}

float ParticlePhase(float g, float b, float c)
{
 float term1 = (1 - c) / 2 * (1 - pow(b, 2)) / pow((1 + 2 * b * cos(g) + pow(b, 2)), 3/2);

 float term2 = (1 + c) / 2 * (1 - pow(b, 2)) / pow((1 - 2 * b * cos(g) + pow(b, 2)), 3/2);

 return term1 + term2;
}

float SHOE(float a, float b0, float h)
{
  return b0 / (1 + 1/h * tan(a/2));
}
float CBOE(float a, float b0_cb, float h_cb)
{
  return b0_cb / (1 + pow((tan(a/2) / h_cb), 2));
}

float HG(float g, float cosTheta)
{
  return (1.0 - g*g) / pow(1.0 + 2.0*g*cosTheta + g*g, 1.5);
}

float OppositionEffect(float a, float b0, float h, float b0_cb, float h_cb)
{
  return SHOE(a, b0, h) + CBOE(a, b0_cb, h_cb);
}

float ChandrasekharMultipleScattering(float NdotL, float omega)
{
  float mu = NdotL;

  float r0 = ( 1 - sqrt(1 - omega )) / ( 1 + (sqrt(1 - omega) ));

  return 1 / ( 1 - omega*mu*(r0 + (1 - 2*r0*mu) * log( (1+mu) / mu)));
}

#endif