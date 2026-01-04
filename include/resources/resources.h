#pragma once

#include <string>

// Enums for easy access
namespace Res
{
  // Shader
  const std::string CORE_SHADER = "core";

  // Mesh
  const std::string CUBE_MESH = "cube";
  const std::string PLANE_MESH = "plane";
  const std::string CIRCLE_MESH = "circle";
  const std::string SPHERE_MESH = "sphere";
  const std::string SUN = "sun";
  const std::string EARTH = "earth";
  const std::string MOON = "moon";

  // Texture
  const std::string EARTH_DIFFUSE = "earth_diffuse";
  const std::string SUN_DIFFUSE = "sun_diffuse";
  const std::string MOON_DIFFUSE = "moon_diffuse";

  // Material
  const std::string EARTH_MATERIAL = "earth_material";
  const std::string SUN_MATERIAL = "sun_material";
  const std::string MOON_MATERIAL = "moon_material";


  // Model
  // const std::string BACKPACK = "backpack";
}

enum Planets{
  SUN = 0, EARTH, MOON
};