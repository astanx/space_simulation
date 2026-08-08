#pragma once

#include <string>

// Enums for easy access
namespace Res
{
  // Shader
  const std::string CORE_SHADER = "core";
  const std::string CORE_TANGENT_SHADER = "core_tangent";
  const std::string SKYBOX_SHADER = "skybox";
  const std::string TRAIL_SHADER = "trail";
  const std::string POINT_SHADOW_SHADER = "point_shadow";
  const std::string DIRECTIONAL_SHADOW_SHADER = "directional_shadow";
  const std::string TEXT_SHADER = "text";
  const std::string HDR_SHADER = "hdr";
  const std::string BLOOM_SHADER = "bloom";
  const std::string BLUR_SHADER = "blur";
  const std::string BLUR_CUBEMAP_SHADER = "blur_cubemap";
  const std::string CUBEMAP_SHADER = "cubemap";
  const std::string CONVOLUTION_SHADER = "convolution";
  const std::string REFLECTION_SHADER = "reflection";
  const std::string DOWNSAMPLE_SHADER = "downsample";
  const std::string UPSAMPLE_SHADER = "upsample";
  const std::string ATMOSPHERE_SHADER = "atmosphere";
  const std::string IMPOSTOR_SHADER = "impostor";
  const std::string POINT_SHADER = "point";

  // Mesh
  const std::string FULLSCREEN_QUAD = "fullscreenquad";
  const std::string CUBE_MESH = "cube";
  const std::string PLANE_MESH = "plane";
  const std::string CIRCLE_MESH = "circle";
  const std::string SPHERE_MESH = "sphere";
  const std::string ASTEROID = "asteroid";

  const std::string SUN_MESH = "sun_mesh";
  const std::string MERCURY_MESH = "mercury_mesh";
  const std::string VENUS_MESH = "venus_mesh";
  const std::string EARTH_MESH = "earth_mesh";
  const std::string MOON_MESH = "moon_mesh";
  const std::string MARS_MESH = "mars_mesh";
  const std::string JUPITER_MESH = "jupiter_mesh";
  const std::string SATURN_MESH = "saturn_mesh";
  const std::string URANUS_MESH = "uranus_mesh";
  const std::string NEPTUNE_MESH = "neptune_mesh";

  const std::string EARTH_ATMOSPHERE_MESH = "earth_atmosphere_mesh";
  const std::string VENUS_ATMOSPHERE_MESH = "venus_atmosphere_mesh";

  const std::string EROS_ASTEROID_MESH = "eros_asteroid_mesh";
  const std::string ITOKAWA_ASTEROID_MESH = "itokawa_asteroid_mesh";
  const std::string BENNU_ASTEROID_MESH = "bennu_asteroid_mesh";
  const std::string RYUGU_ASTEROID_MESH = "ryugu_asteroid_mesh";
  const std::string VESTA_ASTEROID_MESH = "vesta_asteroid_mesh";

  // Texture
  const std::string ASTEROID_DIFFUSE = "asteroid_diffuse";

  const std::string SUN_DIFFUSE = "sun_diffuse";
  const std::string MERCURY_DIFFUSE = "mercury_diffuse";
  const std::string VENUS_DIFFUSE = "venus_diffuse";
  const std::string EARTH_DIFFUSE = "earth_diffuse";
  const std::string MOON_DIFFUSE = "moon_diffuse";
  const std::string MARS_DIFFUSE = "mars_diffuse";
  const std::string JUPITER_DIFFUSE = "jupiter_diffuse";
  const std::string SATURN_DIFFUSE = "saturn_diffuse";
  const std::string URANUS_DIFFUSE = "uranus_diffuse";
  const std::string NEPTUNE_DIFFUSE = "neptune_diffuse";

  const std::string ASTEROID_SPECULAR = "asteroid_specular";

  const std::string SUN_SPECULAR = "sun_specular";
  const std::string MERCURY_SPECULAR = "mercury_specular";
  const std::string VENUS_SPECULAR = "venus_specular";
  const std::string EARTH_SPECULAR = "earth_specular";
  const std::string MOON_SPECULAR = "moon_specular";
  const std::string MARS_SPECULAR = "mars_specular";
  const std::string JUPITER_SPECULAR = "jupiter_specular";
  const std::string SATURN_SPECULAR = "saturn_specular";
  const std::string URANUS_SPECULAR = "uranus_specular";
  const std::string NEPTUNE_SPECULAR = "neptune_specular";

  const std::string ASTEROID_NORMAL = "asteroid_normal";

  const std::string SUN_NORMAL = "sun_normal";
  const std::string MERCURY_NORMAL = "mercury_normal";
  const std::string VENUS_NORMAL = "venus_normal";
  const std::string EARTH_NORMAL = "earth_normal";
  const std::string MOON_NORMAL = "moon_normal";
  const std::string MARS_NORMAL = "mars_normal";
  const std::string JUPITER_NORMAL = "jupiter_normal";
  const std::string SATURN_NORMAL = "saturn_normal";
  const std::string URANUS_NORMAL = "uranus_normal";
  const std::string NEPTUNE_NORMAL = "neptune_normal";

  const std::string EARTH_ATMOSPHERE_DIFFUSE = "earth_atmosphere_diffuse";
  const std::string VENUS_ATMOSPHERE_DIFFUSE = "venus_atmosphere_diffuse";

  const std::string EARTH_NIGHT = "earth_night";

  const std::string EARTH_ROUGHNESS = "earth_roughness";

  // Material
  const std::string ASTEROID_MATERIAL = "asteroid_material";

  const std::string SUN_MATERIAL = "sun_material";
  const std::string MERCURY_MATERIAL = "mercury_material";
  const std::string VENUS_MATERIAL = "venus_material";
  const std::string EARTH_MATERIAL = "earth_material";
  const std::string MOON_MATERIAL = "moon_material";
  const std::string MARS_MATERIAL = "mars_material";
  const std::string JUPITER_MATERIAL = "jupiter_material";
  const std::string SATURN_MATERIAL = "saturn_material";
  const std::string URANUS_MATERIAL = "uranus_material";
  const std::string NEPTUNE_MATERIAL = "neptune_material";

  const std::string EARTH_ATMOSPHERE_MATERIAL = "earth_atmosphere_material";
  const std::string VENUS_ATMOSPHERE_MATERIAL = "venus_atmosphere_material";

  const std::string EROS_ASTEROID_MATERIAL = "eros_asteroid_material";
  const std::string ITOKAWA_ASTEROID_MATERIAL = "itokawa_asteroid_material";
  const std::string BENNU_ASTEROID_MATERIAL = "bennu_asteroid_material";
  const std::string RYUGU_ASTEROID_MATERIAL = "ryugu_asteroid_material";
  const std::string VESTA_ASTEROID_MATERIAL = "vesta_asteroid_material";

  // Models
  const std::string SUN_MODEL = "sun";
  const std::string MERCURY_MODEL = "mercury";
  const std::string VENUS_MODEL = "venus";
  const std::string EARTH_MODEL = "earth";
  const std::string MOON_MODEL = "moon";
  const std::string MARS_MODEL = "mars";
  const std::string JUPITER_MODEL = "jupiter";
  const std::string SATURN_MODEL = "saturn";
  const std::string URANUS_MODEL = "uranus";
  const std::string NEPTUNE_MODEL = "neptune";

  const std::string EARTH_ATMOSPHERE_MODEL = "earth_atmosphere";
  const std::string VENUS_ATMOSPHERE_MODEL = "venus_atmosphere";

  const std::string EROS_ASTEROID_MODEL = "eros_asteroid";
  const std::string ITOKAWA_ASTEROID_MODEL = "itokawa_asteroid";
  const std::string BENNU_ASTEROID_MODEL = "bennu_asteroid";
  const std::string RYUGU_ASTEROID_MODEL = "ryugu_asteroid";
  const std::string VESTA_ASTEROID_MODEL = "vesta_asteroid";

  // Asteroids
  const std::string EROS_ASTEROID = "eros_asteroid";
  const std::string ITOKAWA_ASTEROID = "itokawa_asteroid";
  const std::string BENNU_ASTEROID = "bennu_asteroid";
  const std::string RYUGU_ASTEROID = "ryugu_asteroid";
  const std::string VESTA_ASTEROID = "vesta_asteroid";

  // Contexts
  const std::string MAIN_CONTEXT = "main_context";

  // Programs
  const std::string WISDOM_HOLMAN_INTERGATOR_PROGRAM = "wisdom_holman_intergator";
  const std::string RENDER_QUEUE_PROGRAM = "render_queue";

  // Kernels
  const std::string DRIFT_ANGULAR_KERNEL = "driftAngular";
  const std::string DRIFT_OBJECTS_LINEAR_KERNEL = "driftObjectsLinear";
  const std::string DRIFT_ORBITAL_LINEAR_KERNEL = "driftOrbitalLinear";
  const std::string HALF_KICK_ANGULAR_KERNEL = "halfKickAngular";
  const std::string HALF_KICK_LINEAR_KERNEL = "halfKickLinear";
  const std::string HALF_KICK_KERNEL = "halfKick";

  const std::string LOCAL_SCAN_KERNEL = "scanLocal";
  const std::string GROUP_SCAN_KERNEL = "scanGroup";
  const std::string GROUP_OFFSET_SCAN_KERNEL = "scanGroupOffset";

  const std::string LOD_FULL_LOCAL_SCAN_KERNEL = "scanLocalFullLOD";
  const std::string LOD_IMPOSTOR_LOCAL_SCAN_KERNEL = "scanLocalImpostorLOD";
  const std::string LOD_POINT_LOCAL_SCAN_KERNEL = "scanLocalPointLOD";
  const std::string LOD_FULL_GROUP_SCAN_KERNEL = "scanGroupFullLOD";
  const std::string LOD_IMPOSTOR_GROUP_SCAN_KERNEL = "scanGroupImpostorLOD";
  const std::string LOD_POINT_GROUP_SCAN_KERNEL = "scanGroupPointGroudLOD";
  const std::string LOD_FULL_GROUP_OFFSET_SCAN_KERNEL = "scanGroupOffsetFullLOD";
  const std::string LOD_IMPOSTOR_GROUP_OFFSET_SCAN_KERNEL = "scanGroupOffsetImpostorLOD";
  const std::string LOD_POINT_GROUP_OFFSET_SCAN_KERNEL = "scanGroupOffsetPointLOD";
  const std::string LOD_PASS_KERNEL = "lodPass";

  const std::string PARTITION_OBJECTS_KERNEL = "partitionObjects";
}