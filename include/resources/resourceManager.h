#pragma once

#include "resources/asteroidType.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/materials/material.h"
#include "graphics/mesh.h"
#include "graphics/model.h"

#include "compute/kernel.h"
#include "compute/program.h"
#include "compute/context.h"

#include <unordered_map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <OpenCL/cl.h>

class Primitive;
class ModelRegistry;
struct AsteroidShape;

struct PhongMaterialProperties;

class ResourceManager
{
private:
  ModelRegistry &modelRegistry;

  std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
  std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
  std::unordered_map<std::string, std::unique_ptr<Material>> materials;
  std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
  std::unordered_map<std::string, std::unique_ptr<Model>> models;
  std::unordered_map<std::string, std::unique_ptr<AsteroidType>> asteroids;

  std::unordered_map<std::string, std::unique_ptr<Kernel>> kernels;
  std::unordered_map<std::string, std::unique_ptr<Program>> programs;
  std::unordered_map<std::string, std::unique_ptr<Context>> contexts;

public:
  ResourceManager(ModelRegistry &modelRegistry) : modelRegistry(modelRegistry) {};
  ~ResourceManager() = default;

  // Loaders
  Kernel &LoadKernel(const std::string &name, cl_program program);
  Kernel &LoadKernel(const std::string &storeName, const std::string &kernelName, cl_program program);
  Kernel &LoadKernel(const std::string &name, const std::string &programName);
  Program &LoadProgram(const std::string &name, const std::string &filePath, Context &context);
  Program &LoadProgram(const std::string &name, const std::string &filePath, const std::string &contextName);
  Context &LoadContext(const std::string &name);
  Shader &LoadShader(const std::string &name, const int GLSLmajor, const int GLSLminor, const char *vertexFile, const char *fragmentFile, const char *geometryFile = nullptr);
  Texture &LoadTexture(const std::string &name, const std::string &filePath, GLenum type);
  Material &LoadPhongMaterial(const std::string &name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                              Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture, float shininess);
  Material &LoadPhongMaterial(const std::string &name, PhongMaterialProperties material, Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture);

  Material &LoadPBRMaterial(const std::string &name, Texture *albedoMap, Texture *normalMap, Texture *aoMap, Texture *metallicMap, Texture *roughnessMap, Texture *nightMap, float emissiveStrength, float ao, float metallic, float roughness);

  template <typename T>
  Mesh &LoadMesh(const std::string &name, std::vector<T> *vertexArray, std::vector<GLuint> *indexArray, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);
  template <typename T>
  Mesh &LoadMesh(const std::string &name, std::unique_ptr<Primitive> primitive, VertexLayout layout, GLenum drawMode = GL_TRIANGLES);
  template <typename T>
  AsteroidType &LoadAsteroid(const std::string &name, const std::string &model_name, const std::string &mesh_name, std::unique_ptr<AsteroidShape> shape, Material &material, VertexLayout layout);
  Model &LoadModel(const std::string &name, Material &mat, Mesh &mesh);
  Model &LoadModel(const std::string &name, const std::string &material_name, const std::string &mesh_name);
  // Getters
  Kernel &GetKernel(const std::string &name);
  Program &GetProgram(const std::string &name);
  Context &GetContext(const std::string &name);
  Shader &GetShader(const std::string &name);
  Texture &GetTexture(const std::string &name);
  Material &GetMaterial(const std::string &name);
  Mesh &GetMesh(const std::string &name);
  Model &GetModel(const std::string &name);
  AsteroidType &GetAsteroid(const std::string &name);
  std::vector<Shader *> GetAllShaders();
};

#include "resources/resourceManager.tpp"