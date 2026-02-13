#include "graphics/materials/phongMaterial.h"

#include "graphics/bindings/texture.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

#include "physics/structs/materialProperties.h"

#include <iostream>

// Constructor and Destructor
PhongMaterial::PhongMaterial(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
    Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture, float shininess)
{
  this->ambient = ambient;
  this->diffuse = diffuse;
  this->specular = specular;
  this->diffuseTexture = diffuseTexture;
  this->specularTexture = specularTexture;
  this->normalTexture = normalTexture;
  this->shininess = shininess;
}
PhongMaterial::PhongMaterial(
    MaterialProperties material,
    Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture)
{
  this->ambient = material.ambient;
  this->diffuse = material.diffuse;
  this->specular = material.specular;
  this->diffuseTexture = diffuseTexture;
  this->specularTexture = specularTexture;
  this->normalTexture = normalTexture;
  this->shininess = material.shininess;
}

// Public functions
void PhongMaterial::sendToShader(Shader &program)
{
  program.setVec3f(this->ambient, "material.ambient");
  program.setVec3f(this->diffuse, "material.diffuse");
  program.setVec3f(this->specular, "material.specular");
  program.set1f(this->shininess, "material.shininess");

  // int isTexture = 0;

  if (this->diffuseTexture)
  {
    this->diffuseTexture->bind(TextureBindingPoints::Diffuse);
    program.set1i(TextureBindingPoints::Diffuse, "material.diffuseTexture");
    // isTexture = 1;
  }
  else
  {
    glActiveTexture(GL_TEXTURE0 + TextureBindingPoints::Diffuse);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  if (this->specularTexture)
  {
    this->specularTexture->bind(TextureBindingPoints::Specular);
    program.set1i(TextureBindingPoints::Specular, "material.specularTexture");
    // isTexture = 1;
  }
  else
  {
    glActiveTexture(GL_TEXTURE0 + TextureBindingPoints::Specular);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  if (this->normalTexture)
  {
    this->normalTexture->bind(TextureBindingPoints::Normal);
    program.set1i(TextureBindingPoints::Normal, "material.normalTexture");
    program.set1i(1, "useTBN");
    // isTexture = 1;
  }
  else
  {
    glActiveTexture(GL_TEXTURE0 + TextureBindingPoints::Normal);
    glBindTexture(GL_TEXTURE_2D, 0);
    program.set1i(0, "useTBN");
  }

  // program.set1i(isTexture, "isTexture");
}
