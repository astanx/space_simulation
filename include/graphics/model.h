#pragma once

#include "graphics/mesh.h"
#include <vector>
#include <glm/glm.hpp>

class Material;
class Shader;
class Texture;

class Model
{
private:
  Material *material;
  Texture *overrideTextureDiffuse;
  Texture *overrideTextureSpecular;
  std::vector<Mesh *> meshes;
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;

  void updateUniforms(Shader *shader);

public:
  Model(glm::vec3 position, Material *material,
        std::vector<Mesh *> meshes,
        Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
        glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));

  // OBJ consturctor
  Model(glm::vec3 position, Material *material,
        const char *OBJfile,
        Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
        glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));
  ~Model();

  void render(Shader *shader);
  void scaleBy(const glm::vec3 scale);
  void setPosition(const glm::vec3 &newPosition);
};