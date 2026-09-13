#include "resources/resourceInitializer.h"

#include "debug/logger.h"

#include "resources/manager/resourceManager.h"
#include "resources/resources.h"

#include "graphics/primitives/quad.h"
#include "graphics/primitives/ellipsoid.h"
#include "graphics/primitives/asteroidShape.h"

#include "core/window/windowConfig.h"

#include "physics/constants/constants.h"

#include <filesystem>

// Private functions
LoadedTextures ResourceInitializer::loadTextures(ResourceManager &manager, const std::string &name, const std::string &diffuse_name, const std::string &normal_name, const std::string &night_name, const std::string &roughness_name)
{
  const std::string format = ".png";

  const std::string diffusePath = BASE_TEXTURE_PATH + "diffuse/" + name + format;

  if (!std::filesystem::exists(diffusePath))
    Logger::logFatal("Resource Initializer", "Diffuse texture is not found, skipping the object - " + name);

  LoadedTextures textures;
  textures.diffuse = &manager.LoadTexture(diffuse_name, diffusePath, GL_TEXTURE_2D);

  const std::string roughnessPath = BASE_TEXTURE_PATH + "roughness/" + name + format;
  if (std::filesystem::exists(roughnessPath) && roughness_name != "")
  {
    Logger::logInfo("Resource Initializer", "Found roughness texture for object - " + name);
    textures.roughness = &manager.LoadTexture(roughness_name, roughnessPath, GL_TEXTURE_2D);
  }

  const std::string normalPath = BASE_TEXTURE_PATH + "normal/" + name + format;
  if (std::filesystem::exists(normalPath) && normal_name != "")
  {
    Logger::logInfo("Resource Initializer", "Found normal texture for object - " + name);
    textures.normal = &manager.LoadTexture(normal_name, normalPath, GL_TEXTURE_2D);
  }

  const std::string nightPath = BASE_TEXTURE_PATH + "night/" + name + format;
  if (std::filesystem::exists(nightPath) && night_name != "")
  {
    Logger::logInfo("Resource Initializer", "Found night texture for object - " + name);
    textures.night = &manager.LoadTexture(night_name, nightPath, GL_TEXTURE_2D);
  }

  return textures;
}

void ResourceInitializer::loadHapkePBRMaterial(ResourceManager &manager, const std::string &name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                               float ao, float metallic, float roughness, HapkeParameters params, float emissiveStrength, const std::string &normal_name, const std::string &night_name,
                                               const std::string &roughness_name)
{
  LoadedTextures textures = ResourceInitializer::loadTextures(manager, name, diffuse_name, normal_name, night_name, roughness_name);
  manager.LoadPBRMaterial(material_name, textures.diffuse, textures.normal, nullptr, nullptr, textures.roughness, textures.night, emissiveStrength, ao, metallic, roughness);
}

void ResourceInitializer::loadPBRMaterial(ResourceManager &manager, const std::string &name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                          float ao, float metallic, float roughness, float emissiveStrength, const std::string &normal_name, const std::string &night_name,
                                          const std::string &roughness_name)
{
  LoadedTextures textures = ResourceInitializer::loadTextures(manager, name, diffuse_name, normal_name, night_name, roughness_name);
  manager.LoadPBRMaterial(material_name, textures.diffuse, textures.normal, nullptr, nullptr, textures.roughness, textures.night, emissiveStrength, ao, metallic, roughness);
}

void ResourceInitializer::loadEllipsoid(ResourceManager &manager, const std::string &mesh_name, Radii radii, bool isTangent, int segments)
{
  std::unique_ptr<Ellipsoid> obj = std::make_unique<Ellipsoid>(segments, radii, isTangent);
  if (isTangent)
    manager.LoadMesh<VertexPositionTexcoordNormalTangent>(mesh_name, std::move(obj),
                                                          VertexLayout::PositionNormalTangent);
  else
    manager.LoadMesh<VertexPositionTexcoordNormal>(mesh_name, std::move(obj),
                                                   VertexLayout::NoColor);
}
void ResourceInitializer::loadEllipsoidObject(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                              Radii radii, float ao, float metallic, float roughness, ModelFlags flags, float emissiveStrength, const std::string &normal_name, const std::string &night_name,
                                              const std::string &roughness_name, int segments)
{
  ResourceInitializer::loadPBRMaterial(manager, name, mesh_name, diffuse_name, material_name, ao, metallic, roughness, emissiveStrength, normal_name, night_name, roughness_name);

  bool isTangent = normal_name != "";

  ResourceInitializer::loadEllipsoid(manager, mesh_name, radii, isTangent, segments);

  manager.LoadModel(model_name, material_name, mesh_name, flags);
}
void ResourceInitializer::loadReflectanceAcceptorEllipsoidObject(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                                                 Radii radii, float ao, float metallic, float roughness, ModelFlags flags, float emissiveStrength, const std::string &normal_name, const std::string &night_name,
                                                                 const std::string &roughness_name, int segments)
{
  ResourceInitializer::loadPBRMaterial(manager, name, mesh_name, diffuse_name, material_name, ao, metallic, roughness, emissiveStrength, normal_name, night_name, roughness_name);

  bool isTangent = normal_name != "";

  ResourceInitializer::loadEllipsoid(manager, mesh_name, radii, isTangent, segments);

  manager.LoadReflectanceAcceptorModel(model_name, material_name, mesh_name, flags);
}
void ResourceInitializer::loadHapkeEllipsoidObject(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                                   Radii radii, float ao, float metallic, float roughness, HapkeParameters hapke, const std::string &acceptor_model_name, ModelFlags flags, float emissiveStrength,
                                                   const std::string &normal_name, const std::string &night_name, const std::string &roughness_name, int segments)
{
  ResourceInitializer::loadHapkePBRMaterial(manager, name, mesh_name, diffuse_name, material_name, ao, metallic, roughness, hapke, emissiveStrength, normal_name, night_name, roughness_name);

  bool isTangent = normal_name != "";

  ResourceInitializer::loadEllipsoid(manager, mesh_name, radii, isTangent, segments);

  manager.LoadReflectorModel(model_name, material_name, mesh_name, acceptor_model_name, flags);
}

void ResourceInitializer::loadAsteroidShape(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &material_name,
                                            Texture &albedo, float ao, float metallic, float roughness,
                                            double thetaSteps, double phiSteps, double m, double a, double b, double n1, double n2, double n3)
{
  std::unique_ptr<AsteroidShape> shape = std::make_unique<AsteroidShape>(thetaSteps, phiSteps, m, a, b, n1, n2, n3);

  Material &mat = manager.LoadPBRMaterial(material_name, &albedo, nullptr, nullptr, nullptr, nullptr, nullptr, 0.f, ao, metallic, roughness);
  manager.LoadAsteroid<VertexPositionTexcoordNormal>(name, model_name, mesh_name, std::move(shape), mat, VertexLayout::NoColor);
}

// Public functions
void ResourceInitializer::loadShaders(ResourceManager &manager, const WindowConfig &cfg)
{
  // manager.LoadShader(Res::CORE_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/debug/normal_fragment.glsl", "assets/shaders/debug/normal_geometry.glsl");
  manager.LoadShader(Res::CORE_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/vertex_core.glsl", "assets/shaders/fragment_core.glsl");
  manager.LoadShader(Res::CORE_TANGENT_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/vertex_tangent_core.glsl", "assets/shaders/fragment_tangent_core.glsl");
  manager.LoadShader(Res::SKYBOX_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/skybox/vertex.glsl", "assets/shaders/skybox/fragment.glsl");
  manager.LoadShader(Res::TRAIL_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/trail/vertex.glsl", "assets/shaders/trail/fragment.glsl");
  manager.LoadShader(Res::POINT_SHADOW_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/shadow/point/vertex.glsl", "assets/shaders/shadow/point/fragment.glsl", "assets/shaders/shadow/point/geometry.glsl");
  // manager.LoadShader(Res::DIRECTIONAL_SHADOW_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/shadow/directional/vertex.glsl", "assets/shaders/shadow/directional/fragment.glsl");
  manager.LoadShader(Res::TEXT_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/text/vertex.glsl", "assets/shaders/text/fragment.glsl");
  manager.LoadShader(Res::HDR_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/hdr/vertex.glsl", "assets/shaders/hdr/fragment.glsl");
  manager.LoadShader(Res::BLOOM_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/bloom/vertex.glsl", "assets/shaders/bloom/fragment.glsl");
  manager.LoadShader(Res::BLUR_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/blur/texture/vertex.glsl", "assets/shaders/blur/texture/fragment.glsl");
  manager.LoadShader(Res::BLUR_CUBEMAP_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/blur/cubemap/vertex.glsl", "assets/shaders/blur/cubemap/fragment.glsl", "assets/shaders/blur/cubemap/geometry.glsl");
  manager.LoadShader(Res::CUBEMAP_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/cubemap/vertex.glsl", "assets/shaders/cubemap/fragment.glsl");
  manager.LoadShader(Res::CONVOLUTION_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/convolution/vertex.glsl", "assets/shaders/convolution/fragment.glsl");
  manager.LoadShader(Res::REFLECTION_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/reflector/vertex.glsl", "assets/shaders/reflector/fragment.glsl");
  manager.LoadShader(Res::DOWNSAMPLE_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/sample/down/vertex.glsl", "assets/shaders/sample/down/fragment.glsl");
  manager.LoadShader(Res::UPSAMPLE_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/sample/up/vertex.glsl", "assets/shaders/sample/up/fragment.glsl");
  manager.LoadShader(Res::ATMOSPHERE_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/atmosphere/vertex.glsl", "assets/shaders/atmosphere/fragment.glsl");
  manager.LoadShader(Res::IMPOSTOR_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/impostor/vertex.glsl", "assets/shaders/impostor/fragment.glsl");
  manager.LoadShader(Res::POINT_SHADER, cfg.GLmajor, cfg.GLminor, "assets/shaders/point/vertex.glsl", "assets/shaders/point/fragment.glsl");
}
void ResourceInitializer::loadKernels(ResourceManager &manager)
{
  manager.LoadProgram(Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM, "assets/kernels/wisdomHolman/wisdomHolman.cl", Res::MAIN_CONTEXT);
  manager.LoadKernel(Res::DRIFT_ANGULAR_KERNEL, Res::DRIFT_ANGULAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  manager.LoadKernel(Res::DRIFT_OBJECTS_LINEAR_KERNEL, Res::DRIFT_OBJECTS_LINEAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  manager.LoadKernel(Res::DRIFT_ORBITAL_LINEAR_KERNEL, Res::DRIFT_ORBITAL_LINEAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  manager.LoadKernel(Res::HALF_KICK_LINEAR_KERNEL, Res::HALF_KICK_LINEAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  manager.LoadKernel(Res::HALF_KICK_ANGULAR_KERNEL, Res::HALF_KICK_ANGULAR_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);
  manager.LoadKernel(Res::HALF_KICK_KERNEL, Res::HALF_KICK_KERNEL, Res::WISDOM_HOLMAN_INTERGATOR_PROGRAM);

  manager.LoadProgram(Res::RENDER_QUEUE_PROGRAM, "assets/kernels/render/render.cl", Res::MAIN_CONTEXT);
  manager.LoadKernel(Res::LOD_FULL_LOCAL_SCAN_KERNEL, Res::LOCAL_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_NON_FULL_LOCAL_SCAN_KERNEL, Res::LOCAL_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_IMPOSTOR_LOCAL_SCAN_KERNEL, Res::LOCAL_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_POINT_LOCAL_SCAN_KERNEL, Res::LOCAL_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_FULL_GROUP_SCAN_KERNEL, Res::GROUP_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_NON_FULL_GROUP_SCAN_KERNEL, Res::GROUP_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_IMPOSTOR_GROUP_SCAN_KERNEL, Res::GROUP_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_POINT_GROUP_SCAN_KERNEL, Res::GROUP_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_FULL_GROUP_OFFSET_SCAN_KERNEL, Res::GROUP_OFFSET_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_NON_FULL_GROUP_OFFSET_SCAN_KERNEL, Res::GROUP_OFFSET_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_IMPOSTOR_GROUP_OFFSET_SCAN_KERNEL, Res::GROUP_OFFSET_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_POINT_GROUP_OFFSET_SCAN_KERNEL, Res::GROUP_OFFSET_SCAN_KERNEL, Res::RENDER_QUEUE_PROGRAM);
  manager.LoadKernel(Res::LOD_PASS_KERNEL, Res::LOD_PASS_KERNEL, Res::RENDER_QUEUE_PROGRAM);

  manager.LoadKernel(Res::PARTITION_OBJECTS_KERNEL, Res::PARTITION_OBJECTS_KERNEL, Res::RENDER_QUEUE_PROGRAM);
}
void ResourceInitializer::loadModels(ResourceManager &manager)
{
  ResourceInitializer::loadEllipsoidObject(manager, Res::SUN, Res::SUN_MODEL, Res::SUN_MESH, Res::SUN_DIFFUSE, Res::SUN_MATERIAL, sunRadii, 1.f, 0.f, 0.05f, ModelFlags::Special, sunLuminosity);
  // ResourceInitializer::loadEllipsoidObject(manager, Res::SUN, Res::SUN_DIFFUSE, Res::SUN_MATERIAL, sunRadii, 1.f, 0.f, 0.05f);
  ResourceInitializer::loadEllipsoidObject(manager, Res::MERCURY, Res::MERCURY_MODEL, Res::MERCURY_MESH, Res::MERCURY_DIFFUSE, Res::MERCURY_MATERIAL, mercuryRadii, 0.9f, 0.f, 0.95f, ModelFlags::CastsShadow | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::VENUS, Res::VENUS_MODEL, Res::VENUS_MESH, Res::VENUS_DIFFUSE, Res::VENUS_MATERIAL, venusRadii, 0.9f, 0.f, 0.98f, ModelFlags::CastsShadow | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::VENUS_ATMOSPHERE, Res::VENUS_ATMOSPHERE_MODEL, Res::VENUS_ATMOSPHERE_MESH, Res::VENUS_ATMOSPHERE_DIFFUSE, Res::VENUS_ATMOSPHERE_MATERIAL, venusRadii.scaled(1.01), 1.f, 0.f, 0.05f);
  ResourceInitializer::loadReflectanceAcceptorEllipsoidObject(manager, Res::EARTH, Res::EARTH_MODEL, Res::EARTH_MESH, Res::EARTH_DIFFUSE, Res::EARTH_MATERIAL, earthRadii, 1.f, 0.f, 0.55f, ModelFlags::CastsShadow | ModelFlags::Special, 0.0f, Res::EARTH_NORMAL, Res::EARTH_NIGHT, Res::EARTH_ROUGHNESS);
  ResourceInitializer::loadEllipsoidObject(manager, Res::EARTH_ATMOSPHERE, Res::EARTH_ATMOSPHERE_MODEL, Res::EARTH_ATMOSPHERE_MESH, Res::EARTH_ATMOSPHERE_DIFFUSE, Res::EARTH_ATMOSPHERE_MATERIAL, earthRadii.scaled(1.01), 1.f, 0.f, 0.03f);
  ResourceInitializer::loadHapkeEllipsoidObject(manager, Res::MOON, Res::MOON_MODEL, Res::MOON_MESH, Res::MOON_DIFFUSE, Res::MOON_MATERIAL, moonRadii, 0.95f, 0.f, 0.95f, moonHapkeParameters, Res::EARTH_MODEL, ModelFlags::CastsShadow | ModelFlags::ReflectsLight | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::MARS, Res::MARS_MODEL, Res::MARS_MESH, Res::MARS_DIFFUSE, Res::MARS_MATERIAL, marsRadii, 0.9f, 0.f, 0.9f, ModelFlags::CastsShadow | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::JUPITER, Res::JUPITER_MODEL, Res::JUPITER_MESH, Res::JUPITER_DIFFUSE, Res::JUPITER_MATERIAL, jupiterRadii, 1.f, 0.f, 0.25f, ModelFlags::CastsShadow | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::SATURN, Res::SATURN_MODEL, Res::SATURN_MESH, Res::SATURN_DIFFUSE, Res::SATURN_MATERIAL, saturnRadii, 0.9f, 0.f, 0.85f, ModelFlags::CastsShadow | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::URANUS, Res::URANUS_MODEL, Res::URANUS_MESH, Res::URANUS_DIFFUSE, Res::URANUS_MATERIAL, uranusRadii, 0.94f, 0.f, 0.9f, ModelFlags::CastsShadow | ModelFlags::Special);
  ResourceInitializer::loadEllipsoidObject(manager, Res::NEPTUNE, Res::NEPTUNE_MODEL, Res::NEPTUNE_MESH, Res::NEPTUNE_DIFFUSE, Res::NEPTUNE_MATERIAL, neptuneRadii, 0.9f, 0.f, 0.8f, ModelFlags::CastsShadow | ModelFlags::Special);
}

void ResourceInitializer::loadAsteroids(ResourceManager &manager)
{
  Texture &diff = manager.LoadTexture(Res::ASTEROID_DIFFUSE, BASE_TEXTURE_PATH + "diffuse/asteroid.png", GL_TEXTURE_2D);
  ResourceInitializer::loadAsteroidShape(manager, Res::EROS_ASTEROID, Res::EROS_ASTEROID_MODEL, Res::EROS_ASTEROID_MESH, Res::EROS_ASTEROID_MATERIAL, diff, 0.85f, 0.05f, 0.92f, 48, 32, 4.0, 1.0, 1.0, 2.5, 8.0, 8.0);
  ResourceInitializer::loadAsteroidShape(manager, Res::ITOKAWA_ASTEROID, Res::ITOKAWA_ASTEROID_MODEL, Res::ITOKAWA_ASTEROID_MESH, Res::ITOKAWA_ASTEROID_MATERIAL, diff, 0.9f, 0.08f, 0.95f, 64, 48, 7.0, 0.9, 1.1, 3.0, 12.0, 6.0);
  ResourceInitializer::loadAsteroidShape(manager, Res::BENNU_ASTEROID, Res::BENNU_ASTEROID_MODEL, Res::BENNU_ASTEROID_MESH, Res::BENNU_ASTEROID_MATERIAL, diff, 0.78f, 0.03f, 0.88f, 56, 40, 5.0, 1.0, 0.95, 1.8, 4.0, 10.0);
  ResourceInitializer::loadAsteroidShape(manager, Res::RYUGU_ASTEROID, Res::RYUGU_ASTEROID_MODEL, Res::RYUGU_ASTEROID_MESH, Res::RYUGU_ASTEROID_MATERIAL, diff, 0.82f, 0.06f, 0.85f, 52, 36, 3.0, 1.2, 0.8, 2.2, 5.0, 18.0);
  ResourceInitializer::loadAsteroidShape(manager, Res::VESTA_ASTEROID, Res::VESTA_ASTEROID_MODEL, Res::VESTA_ASTEROID_MESH, Res::VESTA_ASTEROID_MATERIAL, diff, 0.88f, 0.04f, 0.9f, 40, 28, 6.0, 1.0, 1.0, 4.0, 3.0, 15.0);
}

void ResourceInitializer::loadFullscreenQuad(ResourceManager &manager)
{
  manager.LoadMesh<VertexPositionTexcoord>(Res::FULLSCREEN_QUAD, std::make_unique<Quad>(), VertexLayout::PositionTexcoord);
}
