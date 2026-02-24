#include "graphics/materials/phongMaterial.h"

#include "graphics/bindings/texture.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

#include "physics/structs/materialProperties.h"

#include <iostream>

// Constructor and Destructor
PhongMaterial::PhongMaterial(
    glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
    Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture, float shininess, glm::vec3 emissive)
{
  this->ambient = ambient;
  this->diffuse = diffuse;
  this->specular = specular;
  this->diffuseTexture = diffuseTexture;
  this->specularTexture = specularTexture;
  this->normalTexture = normalTexture;
  this->shininess = shininess;
  this->emissive = emissive;
}
PhongMaterial::PhongMaterial(
    PhongMaterialProperties material,
    Texture *diffuseTexture, Texture *specularTexture, Texture *normalTexture)
{
  this->ambient = material.ambient;
  this->diffuse = material.diffuse;
  this->specular = material.specular;
  this->diffuseTexture = diffuseTexture;
  this->specularTexture = specularTexture;
  this->normalTexture = normalTexture;
  this->shininess = material.shininess;
  this->emissive = material.emissive;
}

// Public functions
void PhongMaterial::sendToShader(Shader &program)
{
  program.setVec3f(this->ambient, "phongMaterial.ambient");
  program.setVec3f(this->diffuse, "phongMaterial.diffuse");
  program.setVec3f(this->specular, "phongMaterial.specular");
  program.set1f(this->shininess, "phongMaterial.shininess");
  program.setVec3f(this->emissive, "phongMaterial.emissive");

  // int isTexture = 0;

  this->sendTexture(this->diffuseTexture, program, TextureBindingPoints::Diffuse, "phongMaterial.diffuseTexture");
  this->sendTexture(this->specularTexture, program, TextureBindingPoints::Specular, "phongMaterial.specularTexture");
  this->sendTexture(this->normalTexture, program, TextureBindingPoints::Normal, "phongMaterial.normalTexture");
  if (this->normalTexture)
    program.set1i(1, "useTBN");
  else
    program.set1i(0, "useTBN");


  // program.set1i(isTexture, "isTexture");
}
