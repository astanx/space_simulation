#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;
class Texture;

class Material
{
private:
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  Texture* diffuseTexture;
  Texture* specularTexture;
  float shininess;

public:
  Material(
      glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
      Texture* diffuseTexture, Texture* specularTexture, float shininess);

  

  ~Material();

  void sendToShader(Shader &program);
};
