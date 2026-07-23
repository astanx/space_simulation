#include "resources/resourceManager.h"

#include "resources/asteroidType.h"

#include "debug/logger.h"

#include "graphics/primitives/primitives.h"
#include "graphics/primitives/asteroidShape.h"
#include "graphics/materials/asteroidMaterial.h"
#include "graphics/materials/phongMaterial.h"
#include "graphics/materials/pbrMaterial.h"

#include "physics/structs/materialProperties.h"

#include <iostream>

// Loaders
Kernel &ResourceManager::LoadKernel(const std::string &name, cl_program program)
{
  this->kernels[name] = std::make_unique<Kernel>(name, program);
  return *this->kernels[name];
}
Kernel &ResourceManager::LoadKernel(const std::string &storeName, const std::string &kernelName, cl_program program)
{
  this->kernels[storeName] = std::make_unique<Kernel>(kernelName, program);
  return *this->kernels[storeName];
}
Kernel &ResourceManager::LoadKernel(const std::string &name, const std::string &programName)
{
  this->kernels[name] = std::make_unique<Kernel>(name, this->GetProgram(programName).get());
  return *this->kernels[name];
}
Program &ResourceManager::LoadProgram(const std::string &name, const std::string &filePath, Context &context)
{
  this->programs[name] = std::make_unique<Program>(filePath, context);
  return *this->programs[name];
}
Program &ResourceManager::LoadProgram(const std::string &name, const std::string &filePath, const std::string &contextName)
{
  this->programs[name] = std::make_unique<Program>(filePath, this->GetContext(contextName));
  return *this->programs[name];
}
Context &ResourceManager::LoadContext(const std::string &name)
{
  this->contexts[name] = std::make_unique<Context>();
  return *this->contexts[name];
}
Shader &ResourceManager::LoadShader(const std::string &name, const int GLSLmajor, const int GLSLminor, const char *vertexFile, const char *fragmentFile, const char *geometryFile)
{
  this->shaders[name] = std::make_unique<Shader>(GLSLmajor, GLSLminor, vertexFile, fragmentFile, geometryFile);
  return *this->shaders[name];
}

Texture &ResourceManager::LoadTexture(const std::string &name, const std::string &filePath, GLenum type)
{
  this->textures[name] = std::make_unique<Texture>(filePath, type);
  return *this->textures[name];
}

Material &ResourceManager::LoadPhongMaterial(const std::string &name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                                             Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture, float shininess)
{
  this->materials[name] = std::make_unique<PhongMaterial>(ambient, diffuse, specular, diffuseTexture, specularTexture, normalTexture, shininess);
  return *this->materials[name];
}
Material &ResourceManager::LoadPhongMaterial(const std::string &name, PhongMaterialProperties material,
                                             Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture)
{
  this->materials[name] = std::make_unique<PhongMaterial>(material, diffuseTexture, specularTexture, normalTexture);
  return *this->materials[name];
}

Material &ResourceManager::LoadPBRMaterial(const std::string &name, Texture *albedoMap, Texture *normalMap, Texture *aoMap, Texture *metallicMap, Texture *roughnessMap, Texture *nightMap, float emissiveStrength, float ao, float metallic, float roughness)
{
  this->materials[name] = std::make_unique<PBRMaterial>(albedoMap, normalMap, aoMap, metallicMap, roughnessMap, nightMap, emissiveStrength, ao, metallic, roughness);
  return *this->materials[name];
}

// Getters
Kernel &ResourceManager::GetKernel(const std::string &name)
{
  auto it = this->kernels.find(name);
  if (it == this->kernels.end())
    Logger::logFatal("Resource manager", "Kernel not found: " + name);
  return *it->second;
}
Program &ResourceManager::GetProgram(const std::string &name)
{
  auto it = this->programs.find(name);
  if (it == this->programs.end())
    Logger::logFatal("Resource manager", "Program not found: " + name);
  return *it->second;
}
Context &ResourceManager::GetContext(const std::string &name)
{
  auto it = this->contexts.find(name);
  if (it == this->contexts.end())
    Logger::logFatal("Resource manager", "Context not found: " + name);
  return *it->second;
}
Shader &ResourceManager::GetShader(const std::string &name)
{
  auto it = this->shaders.find(name);
  if (it == this->shaders.end())
    Logger::logFatal("Resource manager", "Shader not found: " + name);
  return *it->second;
}
Texture &ResourceManager::GetTexture(const std::string &name)
{
  auto it = this->textures.find(name);
  if (it == this->textures.end())
    Logger::logFatal("Resource manager", "Texture not found: " + name);
  return *it->second;
}
Material &ResourceManager::GetMaterial(const std::string &name)
{
  auto it = this->materials.find(name);
  if (it == this->materials.end())
    Logger::logFatal("Resource manager", "Material not found: " + name);
  return *it->second;
}
Mesh &ResourceManager::GetMesh(const std::string &name)
{
  auto it = this->meshes.find(name);
  if (it == this->meshes.end())
    Logger::logFatal("Resource manager", "Mesh not found: " + name);
  return *it->second;
}
AsteroidType &ResourceManager::GetAsteroid(const std::string &name)
{
  auto it = this->asteroids.find(name);
  if (it == this->asteroids.end())
    Logger::logFatal("Resource manager", "Asteroid not found: " + name);
  return *it->second;
}
std::vector<Shader *> ResourceManager::GetAllShaders()
{
  std::vector<Shader *> shaders;

  for (const auto &pair : this->shaders)
    shaders.push_back(pair.second.get());

  return shaders;
}