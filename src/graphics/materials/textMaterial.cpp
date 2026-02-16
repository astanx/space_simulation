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
  this->diffuseTexture->activate(TextureBindingPoints::Diffuse);
  this->diffuseTexture->bind();
  program.set1i(TextureBindingPoints::Diffuse, "text");
}
