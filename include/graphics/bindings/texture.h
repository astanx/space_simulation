#pragma once

// max - 16
namespace TextureBindingPoints
{
  constexpr unsigned int Diffuse = 0;
  constexpr unsigned int Specular = 1;
  constexpr unsigned int Normal = 2;
  constexpr unsigned int Skybox = 3;
  constexpr unsigned int PointShadow = 4;
  constexpr unsigned int DirectionalShadow = 5;
  constexpr unsigned int HDRColorBuffer = 6;
  constexpr unsigned int Bloom = 7;
  constexpr unsigned int AoMap = 8;
  constexpr unsigned int RoughnessMap = 9;
  constexpr unsigned int MetallicMap = 10;
  constexpr unsigned int EquirectangularMap = 11;
  constexpr unsigned int EnvironmentMap = 12;
  constexpr unsigned int IrradianceMap = 13;
  constexpr unsigned int Blur = 14;
  constexpr unsigned int DepthMap = 15;
  constexpr unsigned int NightMap = 16;
  constexpr unsigned int Emissive = 17;
}
