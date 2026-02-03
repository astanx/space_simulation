#include "graphics/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_TGA
#define STBI_ONLY_BMP
#define STBI_ONLY_HDR
#define STBI_ONLY_TIFF
#include "external/stb_image.h"
#include <iostream>

// Private functions

// Constructor and Destructor
Texture::Texture(const char *fileName, GLenum type)
{
  this->type = type;
  unsigned char *image = stbi_load(fileName, &this->width, &this->height, 0, 4);

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
    std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD: " << fileName << std::endl;
  }

  glActiveTexture(0);
  glBindTexture(type, 0);
  stbi_image_free(image);
}

Texture::Texture(GLsizei width, GLsizei height, GLenum type, const void *pixels)
{
  this->type = type;

  glGenTextures(1, &this->id);
  glBindTexture(type, this->id);

  glTexImage2D(
      type,
      0,
      GL_RED,
      width,
      height,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      pixels);

  glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(0);
  glBindTexture(type, 0);
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
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(this->type, 0);
}

void Texture::loadFromFile(const char *fileName)
{
  if (this->id)
    glDeleteTextures(1, &this->id);
  unsigned char *image = stbi_load(fileName, &this->width, &this->height, 0, 4);

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
    std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD_FROM_FILE: " << fileName << std::endl;
  }

  glActiveTexture(0);
  glBindTexture(this->type, 0);
  stbi_image_free(image);
}