#include "graphics/materials/asteroidMaterial.h"

#include "graphics/bindings/texture.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

// Constructor
AsteroidMaterial::AsteroidMaterial(Texture &diffuseTexture)
{
  this->diffuseTexture = &diffuseTexture;
}

// Public functions
void AsteroidMaterial::sendToShader(Shader &program)
{
  this->diffuseTexture->activate(TextureBindingPoints::Diffuse);
  this->diffuseTexture->bind();
  program.set1i(TextureBindingPoints::Diffuse, "diffuseTexture");
}
