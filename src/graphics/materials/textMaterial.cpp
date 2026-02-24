#include "graphics/materials/textMaterial.h"

#include "graphics/bindings/texture.h"

#include "graphics/shader.h"
#include "graphics/texture.h"

// Constructor
TextMaterial::TextMaterial(Texture *diffuseTexture)
{
  this->diffuseTexture = diffuseTexture;
}

// Public functions
void TextMaterial::sendToShader(Shader &program)
{
  this->sendTexture(this->diffuseTexture, program, TextureBindingPoints::Diffuse, "text");
}
