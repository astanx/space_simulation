#include "graphics/texture.h"

#include <SOIL2/SOIL2.h>
#include <iostream>

// Private functions

// Constructor and Destructor
Texture::Texture(const char *fileName, GLenum type)
{
  this->type = type;
  unsigned char *image = SOIL_load_image(fileName, &this->width, &this->height, 0, SOIL_LOAD_RGBA);

  glGenTextures(1, &this->id);
  glBindTexture(type, this->id);

  glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (image)
  {
    glTexImage2D(type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(type);
  }
  else
  {
    std::cout << "Failed to load texture: " << fileName << std::endl;
  }

  glActiveTexture(0);
  glBindTexture(type, 0);
  SOIL_free_image_data(image);
}

Texture::~Texture()
{
  glDeleteTextures(1, &this->id);
}

// Public functions
void Texture::bind(const GLint textureUnit)
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(this->type, this->id);
}

void Texture::unbind()
{
  glActiveTexture(0);
  glBindTexture(this->type, 0);
}

void Texture::loadFromFile(const char *fileName)
{
  if (this->id)
    glDeleteTextures(1, &this->id);
  unsigned char *image = SOIL_load_image(fileName, &this->width, &this->height, 0, SOIL_LOAD_RGBA);

  glGenTextures(1, &this->id);
  glBindTexture(this->type, this->id);

  glTexParameteri(this->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(this->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (image)
  {
    glTexImage2D(this->type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(this->type);
  }
  else
  {
    std::cout << "Failed to load texture from file: " << fileName << std::endl;
  }

  glActiveTexture(0);
  glBindTexture(this->type, 0);
  SOIL_free_image_data(image);
}