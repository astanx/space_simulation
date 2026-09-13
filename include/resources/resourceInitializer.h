#pragma once

#include "graphics/model.h"

#include <string>

class Texture;
class ResourceManager;
struct WindowConfig;
struct Radii;
struct HapkeParameters;

struct LoadedTextures
{
  Texture *diffuse = nullptr;
  Texture *roughness = nullptr;
  Texture *normal = nullptr;
  Texture *night = nullptr;
};

const std::string BASE_TEXTURE_PATH = "assets/textures/";

class ResourceInitializer
{
private:
  static LoadedTextures loadTextures(ResourceManager &manager, const std::string &name, const std::string &diffuse_name, const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "");
  static void loadEllipsoid(ResourceManager &manager, const std::string &mesh_name, Radii radii, bool isTangent = false, int segments = 32);

  static void loadHapkePBRMaterial(ResourceManager &manager, const std::string &name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                   float ao, float metallic, float roughness, HapkeParameters params, float emissiveStrength = 0.f, const std::string &normal_name = "", const std::string &night_name = "",
                                   const std::string &roughness_name = "");

  static void loadPBRMaterial(ResourceManager &manager, const std::string &name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                              float ao, float metallic, float roughness, float emissiveStrength = 0.f, const std::string &normal_name = "", const std::string &night_name = "",
                              const std::string &roughness_name = "");
  static void loadEllipsoidObject(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                  Radii radii, float ao, float metallic, float roughness, ModelFlags flags = ModelFlags::None, float emissiveStrength = 0.0f,
                                  const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "", int segments = 32);
  static void loadReflectanceAcceptorEllipsoidObject(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                                     Radii radii, float ao, float metallic, float roughness, ModelFlags flags = ModelFlags::None, float emissiveStrength = 0.0f,
                                                     const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "", int segments = 32);
  static void loadHapkeEllipsoidObject(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &diffuse_name, const std::string &material_name,
                                       Radii radii, float ao, float metallic, float roughness, HapkeParameters hapke, const std::string &acceptor_model_name, ModelFlags flags = ModelFlags::None, float emissiveStrength = 0.0f,
                                       const std::string &normal_name = "", const std::string &night_name = "", const std::string &roughness_name = "", int segments = 32);

  static void loadAsteroidShape(ResourceManager &manager, const std::string &name, const std::string &model_name, const std::string &mesh_name, const std::string &material_name,
                                Texture &albedo, float ao, float metallic, float roughness,
                                double thetaSteps, double phiSteps, double m, double a, double b, double n1, double n2, double n3);

public:
  static void loadShaders(ResourceManager &manager, const WindowConfig &cfg);
  static void loadKernels(ResourceManager &manager);
  static void loadModels(ResourceManager &manager);
  static void loadAsteroids(ResourceManager &manager);
  static void loadFullscreenQuad(ResourceManager &manager);
};