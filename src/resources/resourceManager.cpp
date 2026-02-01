#include "resources/resourceManager.h"
#include "graphics/primitives/primitives.h"
#include "graphics/materials/asteroidMaterial.h"
#include "graphics/materials/phongMaterial.h"

#include <iostream>

// Loaders
Shader *ResourceManager::LoadShader(const std::string &name, const int GLSLmajor, const int GLSLminor, const char *vertexFile, const char *fragmentFile, const char *geometryFile)
{
  this->shaders[name] = std::make_unique<Shader>(GLSLmajor, GLSLminor, vertexFile, fragmentFile, geometryFile);
  return this->shaders[name].get();
}

Texture *ResourceManager::LoadTexture(const std::string &name, const char *filePath, GLenum type)
{
  this->textures[name] = std::make_unique<Texture>(filePath, type);
  return this->textures[name].get();
}

Material *ResourceManager::LoadPhongMaterial(const std::string &name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                                             Texture *diffuseTexture, Texture *specularTexture, float shininess)
{
  this->materials[name] = std::make_unique<PhongMaterial>(ambient, diffuse, specular, diffuseTexture, specularTexture, shininess);
  return this->materials[name].get();
}
Material *ResourceManager::LoadPhongMaterial(const std::string &name, MaterialProperties material,
                                             Texture *diffuseTexture, Texture *specularTexture)
{
  this->materials[name] = std::make_unique<PhongMaterial>(material, diffuseTexture, specularTexture);
  return this->materials[name].get();
}
Material *ResourceManager::LoadAsteroidMaterial(const std::string &name, Texture *diffuseTexture)
{
  this->materials[name] = std::make_unique<AsteroidMaterial>(diffuseTexture);
  return this->materials[name].get();
}

Mesh *ResourceManager::LoadMesh(const std::string &name, Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, VertexLayout layout, GLenum drawMode)
{
  this->meshes[name] = std::make_unique<Mesh>(vertexArray, nrOfVertices, indexArray, nrOfIndices, layout, drawMode);
  return this->meshes[name].get();
}
Mesh *ResourceManager::LoadMesh(const std::string &name, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode)
{
  this->meshes[name] = std::make_unique<Mesh>(std::move(primitive), layout, drawMode);
  return this->meshes[name].get();
}
Mesh *ResourceManager::LoadMesh(const std::string &name, const Mesh &obj)
{
  this->meshes[name] = std::make_unique<Mesh>(obj);
  return this->meshes[name].get();
}

// Getters
Shader *ResourceManager::GetShader(const std::string &name)
{
  auto it = this->shaders.find(name);
  if (it == this->shaders.end())
  {
    std::cerr << "ERROR::RESOURCE_MANAGER::SHADER_NOT_FOUND: '" << name << std::endl;
    assert(false && "Shader not loaded!");
    return nullptr;
  }
  return it->second.get();
}
Texture *ResourceManager::GetTexture(const std::string &name)
{
  auto it = this->textures.find(name);
  if (it == this->textures.end())
  {
    std::cerr << "ERROR::RESOURCE_MANAGER::TEXTURE_NOT_FOUND: '" << name << std::endl;
    assert(false && "Texture not loaded!");
    return nullptr;
  }
  return it->second.get();
}
Material *ResourceManager::GetMaterial(const std::string &name)
{
  auto it = this->materials.find(name);
  if (it == this->materials.end())
  {
    std::cerr << "ERROR::RESOURCE_MANAGER::MATERIAL_NOT_FOUND: '" << name << std::endl;
    assert(false && "Material not loaded!");
    return nullptr;
  }
  return it->second.get();
}
Mesh *ResourceManager::GetMesh(const std::string &name)
{
  auto it = this->meshes.find(name);
  if (it == this->meshes.end())
  {
    std::cerr << "ERROR::RESOURCE_MANAGER::MESH_NOT_FOUND: '" << name << std::endl;
    assert(false && "Mesh not loaded!");
    return nullptr;
  }
  return it->second.get();
}