#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include "shader.h"

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>


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
    GLint diffuseTexture, GLint specularTexture, float shininess
  )
  { 
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->diffuseTexture = diffuseTexture;
    this->specularTexture = specularTexture;
    this->shininess = shininess;
  }

  ~Material(){}

  void sendToShader(Shader& program)
  {
    program.setVec3f(this->ambient, "material.ambient");
    program.setVec3f(this->diffuse, "material.diffuse");
    program.setVec3f(this->specular, "material.specular");
    program.set1i(this->diffuseTexture, "material.diffuseTexture");
    program.set1i(this->specularTexture, "material.specularTexture");
    program.set1f(this->shininess, "material.shininess");
  }
};

