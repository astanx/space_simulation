#version 410

#include "constants/constants.glsl"

layout (location = 0) out vec4 fs_color;
layout (location = 1) out vec4 fs_emissive;

in VS_OUT {
  vec3 vs_position;
  vec2 vs_texcoord;
  vec3 vs_normal;
} fs_in;

uniform int npressure;
uniform float pressure[64];

uniform sampler3D humidityTexture;
uniform sampler3D densityTexture;
uniform sampler3D temperatureTexture;
uniform sampler3D geopotentialTexture;
uniform sampler3D cloudTexture;

void main()
{
  float radius = length(fs_in.vs_position);
  
  float lat = asin(fs_in.vs_position.y / radius);
  float lon = atan(fs_in.vs_position.z, fs_in.vs_position.x);

  float u = (lon + PI) / (2.0 * PI);
  float v = (lat + PI * 0.5) / PI;

  for (int npres = 0; npres < npressure; npres++)
  {
    float pres = pressure[npres];
    float w = float(npres) / float(npressure - 1);

    vec3 uvw = vec3(u, v, w);

    float humidity = texture(humidityTexture, uvw).r;
    float cloud = texture(cloudTexture, uvw).r;
    float temperature = texture(temperatureTexture, uvw).r;
    float geopotential = texture(geopotentialTexture, uvw).r;
    float density = texture(densityTexture, uvw).r;
  }


  fs_color = result;
  fs_emissive = vec4(vec3(0.0), 1.0);
}