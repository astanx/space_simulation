#include "graphics/texture.h"

#include "graphics/state/scopedTexture.h"

#include "debug/logger.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_TGA
#define STBI_ONLY_BMP
#define STBI_ONLY_HDR
#define STBI_ONLY_TIFF
#include "external/stb_image.h"

#include <iostream>

// Constructor and Destructor
Texture::Texture(const std::string &fileName, GLenum target)
{
  this->target = target;
  unsigned char *image = stbi_load(fileName.c_str(), &this->width, &this->height, 0, 4);

  glGenTextures(1, &this->id);
  glBindTexture(target, this->id);

  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (image)
  {
    GL_CALL(glTexImage2D(target, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
    GL_CALL(glGenerateMipmap(target));
  }
  else
    Logger::logError("Texture", "Failed to load file - " + fileName);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(target, 0);
  stbi_image_free(image);
}

Texture::Texture(GLsizei width, GLsizei height, GLenum target, GLint internalFormat, GLenum format, GLenum type, const void *pixels)
{
  this->target = target;

  glGenTextures(1, &this->id);

  glBindTexture(target, this->id);

  GL_CALL(glTexImage2D(
      target,
      0,
      internalFormat,
      width,
      height,
      0,
      format,
      type,
      pixels));

      
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(target, 0);
}

Texture::Texture(GLenum target)
{
  glGenTextures(1, &this->id);
  this->target = target;

  glBindTexture(this->target, this->id);
  glBindTexture(this->target, 0);
}

Texture::~Texture()
{
  glDeleteTextures(1, &this->id);
}

// Public functions
void Texture::bind() const
{
  if (glIsTexture(this->id))
  {
    glBindTexture(this->target, this->id);
  }
  else
    Logger::logError("Texture", "No texture to bind");
}

void Texture::unbind(const GLint textureUnit) const
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(this->target, 0);
}

void Texture::activate(const GLint textureUnit) const
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
}

void Texture::loadFromFile(const std::string &fileName)
{
  if (this->id)
    glDeleteTextures(1, &this->id);
  unsigned char *image = stbi_load(fileName.c_str(), &this->width, &this->height, 0, 4);

  glGenTextures(1, &this->id);
  glBindTexture(this->target, this->id);

  glTexParameteri(this->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(this->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(this->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(this->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (image)
  {
    GL_CALL(glTexImage2D(this->target, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
    GL_CALL(glGenerateMipmap(this->target));
  }
  else
  {
    Logger::logError("Texture", "Failed to load from file - " + fileName);
    return;
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(this->target, 0);
  stbi_image_free(image);
}