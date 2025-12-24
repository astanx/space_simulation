#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
class Shader;

class Material
{
private:
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  GLint diffuseTexture;
  GLint specularTexture;
  float shininess;

public:
  Material(
      glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
      GLint diffuseTexture, GLint specularTexture, float shininess);

  ~Material();

  void sendToShader(Shader &program);
};
