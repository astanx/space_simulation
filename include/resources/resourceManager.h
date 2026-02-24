#pragma once

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/materials/material.h"
#include "graphics/mesh.h"

#include <unordered_map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Primitive;
class AsteroidMaterial;

struct PhongMaterialProperties;

class ResourceManager
{
private:
  std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
  std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
  std::unordered_map<std::string, std::unique_ptr<Material>> materials;
  std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;

public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  // Loaders
  Shader &LoadShader(const std::string &name, const int GLSLmajor, const int GLSLminor, const char *vertexFile, const char *fragmentFile, const char *geometryFile = nullptr);
  Texture &LoadTexture(const std::string &name, const std::string &filePath, GLenum type);
  Material &LoadPhongMaterial(const std::string &name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                              Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture, float shininess);
  Material &LoadPhongMaterial(const std::string &name, PhongMaterialProperties material, Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture);

  Material &LoadAsteroidMaterial(const std::string &name, Texture &diffuseTexture);

  Material &LoadPBRMaterial(const std::string &name, Texture *albedoMap, Texture *normalMap, Texture *aoMap, Texture *metallicMap, Texture *roughnessMap, float emissiveStrength);

  Mesh &LoadMesh(const std::string &name, Vertex *vertexArray, const unsigned &nrOfVertices, GLuint *indexArray, const unsigned &nrOfIndices, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);
  Mesh &LoadMesh(const std::string &name, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);
  Mesh &LoadMesh(const std::string &name, const Mesh &obj);

  // Getters
  Shader &GetShader(const std::string &name);
  Texture &GetTexture(const std::string &name);
  Material &GetMaterial(const std::string &name);
  Mesh &GetMesh(const std::string &name);
  std::vector<Shader *> GetAllShaders();
};