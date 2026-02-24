#include "graphics/materials/material.h"

#include "graphics/texture.h"
#include "graphics/shader.h"

void Material::sendTexture(Texture *texture, Shader &program, unsigned int bindingPoint, const std::string &name)
{
  if (texture)
  {
    texture->activate(bindingPoint);
    texture->bind();
    program.set1i(bindingPoint, name);
  }
  else
  {
    glActiveTexture(GL_TEXTURE0 + bindingPoint);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}