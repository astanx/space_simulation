#pragma once

#include <vector>
#include <glm/glm.hpp>

class Material;
class Mesh;
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

  void updateUniforms(Shader* shader);
public:
  Model(glm::vec3 position, Material *material,
        std::vector<Mesh *>& meshes,
        Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular);
  ~Model();

  void render(Shader* shader);
  void scaleBy(const glm::vec3 scale);
};