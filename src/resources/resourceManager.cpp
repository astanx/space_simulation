#include "resources/resourceManager.h"

#include "debug/logger.h"

#include "graphics/primitives/primitives.h"
#include "graphics/materials/asteroidMaterial.h"
#include "graphics/materials/phongMaterial.h"

#include "physics/structs/materialProperties.h"

#include <iostream>

// Loaders
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
Material &ResourceManager::LoadPhongMaterial(const std::string &name, MaterialProperties material,
                                             Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture)
{
  this->materials[name] = std::make_unique<PhongMaterial>(material, diffuseTexture, specularTexture, normalTexture);
  return *this->materials[name];
}
Material &ResourceManager::LoadAsteroidMaterial(const std::string &name, Texture &diffuseTexture)
{
  this->materials[name] = std::make_unique<AsteroidMaterial>(diffuseTexture);
  return *this->materials[name];
}

Mesh &ResourceManager::LoadMesh(const std::string &name, Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, VertexLayout layout, GLenum drawMode)
{
  this->meshes[name] = std::make_unique<Mesh>(vertexArray, nrOfVertices, indexArray, nrOfIndices, layout, drawMode);
  return *this->meshes[name];
}
Mesh &ResourceManager::LoadMesh(const std::string &name, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->meshes[name] = std::make_unique<Mesh>(std::move(primitive), layout, drawMode);
  return *this->meshes[name];
}
Mesh &ResourceManager::LoadMesh(const std::string &name, const Mesh &obj)
{
  this->meshes[name] = std::make_unique<Mesh>(obj);
  return *this->meshes[name];
}

// Getters
Shader &ResourceManager::GetShader(const std::string &name)
{
  auto it = this->shaders.find(name);
  if (it == this->shaders.end())
    throw std::runtime_error("[Resource manager] RUNTIME ERROR: Shader not found");
  return *it->second;
}
Texture &ResourceManager::GetTexture(const std::string &name)
{
  auto it = this->textures.find(name);
  if (it == this->textures.end())
    throw std::runtime_error("[Resource manager] RUNTIME ERROR: Texture not found");

  return *it->second;
}
Material &ResourceManager::GetMaterial(const std::string &name)
{
  auto it = this->materials.find(name);
  if (it == this->materials.end())
    throw std::runtime_error("[Resource manager] RUNTIME ERROR: Material not found");

  return *it->second;
}
Mesh &ResourceManager::GetMesh(const std::string &name)
{
  auto it = this->meshes.find(name);
  if (it == this->meshes.end())
    throw std::runtime_error("[Resource manager] RUNTIME ERROR: Mesh not found");

  return *it->second;
}