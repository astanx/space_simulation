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
  this->sendTexture(this->diffuseTexture, program, TextureBindingPoints::Diffuse, "diffuseTexture");
}
