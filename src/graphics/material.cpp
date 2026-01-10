#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

// Private functions

// Constructor and Destructor
Material::Material(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
    Texture *diffuseTexture, Texture *specularTexture, float shininess)
{
  this->ambient = ambient;
  this->diffuse = diffuse;
  this->specular = specular;
  this->diffuseTexture = diffuseTexture;
  this->specularTexture = specularTexture;
  this->shininess = shininess;
}

Material::~Material() {}

// Public functions
void Material::sendToShader(Shader &program)
{
  program.setVec3f(this->ambient, "material.ambient");
  program.setVec3f(this->diffuse, "material.diffuse");
  program.setVec3f(this->specular, "material.specular");
  program.set1f(this->shininess, "material.shininess");

  int isTexture = 0;

  if (this->diffuseTexture)
  {
    this->diffuseTexture->bind(0);
    program.set1i(0, "material.diffuseTexture");
    isTexture = 1;
  }
  else
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  if (this->specularTexture)
  {
    this->specularTexture->bind(1);
    program.set1i(1, "material.specularTexture");
    isTexture = 1;
  }
  else
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  program.set1i(isTexture, "isTexture");
}

