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
  this->diffuseTexture->bind(TextureBindingPoints::Diffuse);
  program.set1i(0, "diffuseTexture");
}
