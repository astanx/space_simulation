#version 410

// does not work will work on it later

#include "constants/constants.glsl"
#include "ubo/pbr_point_light.glsl"
#include "ubo/camera.glsl"

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} fs_in;

uniform int npressure;
uniform float pressureLevels[64];

uniform float g; // gravitational acceleration
uniform float g_s; // forward scattering strength

uniform sampler3D humidityTexture;
uniform sampler3D densityTexture;
uniform sampler3D temperatureTexture;
uniform sampler3D geopotentialTexture;
uniform sampler3D cloudTexture;
uniform sampler3D liquidContentTexture;
uniform sampler3D iceContentTexture;


float calculateMolecularNumberDensity(float pressure, float temperature)
{
  const float k_b = 1.380649e-23;
  return pressure / (k_b * temperature);
}

float rayleighScatteringCoefficient(float lambda, float n)
{
  return n / pow(lambda, 4);
}

float cloudScattering(float density)
{
  float p = 1000.0; // water density
  float r = 10e-6; // water droplet radius
  return 3 * density / (2 * PI * r * p); 
}

vec3 calculateTotalExtinction(vec3 lambda, float n, float b_m)
{
  vec3 res;
  res.r = rayleighScatteringCoefficient(lambda.r, n) + b_m;
  res.g = rayleighScatteringCoefficient(lambda.g, n) + b_m;
  res.b = rayleighScatteringCoefficient(lambda.b, n) + b_m;
  return res;
}

void main()
{
  float radius = length(fs_in.vs_position);
  
  float lat = asin(fs_in.vs_position.y / radius);
  float lon = atan(fs_in.vs_position.z, fs_in.vs_position.x);

  float u = (lon + PI) / (2.0 * PI);
  float v = (lat + PI * 0.5) / PI;

  vec3 result;
  float opacity;
vec3 lightLambda = vec3(
    680e-9, // red
    550e-9, // green
    440e-9  // blue
);

  vec3 S = pbrPointLight.position - fs_in.vs_position;
  vec3 V = vec3(camPosition) - fs_in.vs_position;

  float cosTheta = dot(normalize(S), normalize(V));

  for (int npres = 0; npres < npressure; npres++)
  {
    float pressure = pressureLevels[npres];
    float w = float(npres) / float(npressure - 1);

    vec3 uvw = vec3(u, v, w);

    float humidity = texture(humidityTexture, uvw).r;
    float cloud = texture(cloudTexture, uvw).r;
    float temperature = texture(temperatureTexture, uvw).r;
    float geopotential = texture(geopotentialTexture, uvw).r;
    float density = texture(densityTexture, uvw).r;
    float iceContent = texture(iceContentTexture, uvw).r;
    float liquidContent = texture(liquidContentTexture, uvw).r;

    float h = geopotential / g;

    float dh;
    float h_neighbor;
    if (npres < npressure - 1)
    { 
      float w = float(npres + 1) / float(npressure - 1);
      h_neighbor = texture(geopotentialTexture, vec3(u,v,w)).r;
    }
    else 
    {
      float w = float(npres - 1) / float(npressure - 1);
      h_neighbor = texture(geopotentialTexture, vec3(u,v,w)).r;
    }

    dh = abs(h - h_neighbor);

    float n = calculateMolecularNumberDensity(pressure, temperature);

    float b_m = cloudScattering(density);
    vec3 b_ext = calculateTotalExtinction(lightLambda, n, b_m);
    float T = -log(1.0 - cloud);

    float intensity = pbrPointLight.luminosity / (4.0 * PI * PI * pbrPointLight.radius * pbrPointLight.radius);

    vec3 radiance = pbrPointLight.color * intensity;

    vec3 L = radiance * T;

    float P_r = 3 * (1 + cosTheta * cosTheta) / (16 * PI);
    float P_m = (1 - g_s * g_s) / (4 * PI * pow(1 + g_s * g_s - 2 * g_s * cosTheta, 1.5));

    vec3 L_r = L * b_ext * P_r;
    vec3 L_m = L * b_m * P_m;

    opacity += cloud;

    result += (L_r + L_m);
  }
  opacity = clamp(opacity, 0.0, 1.0);
  if (opacity < 0.2) discard;

  fs_color = vec4(result, opacity);
  fs_emissive = vec4(vec3(0.0), 1.0);
}