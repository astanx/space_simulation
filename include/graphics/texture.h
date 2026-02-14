#pragma once

#include <GL/glew.h>

#include <string>

class Texture
{
private:
  GLuint id = 0;
  int width;
  int height;
  GLenum target;

public:
  Texture(const std::string &fileName, GLenum target);
  Texture(GLsizei width, GLsizei height, GLenum target, GLint internalFormat, GLenum format, GLenum type, const void *pixels = nullptr);

  ~Texture();

  inline GLuint getId() const { return this->id; };

  void bind(const GLint textureUnit);

  void unbind(const GLint textureUnit);

  void loadFromFile(const std::string &fileName);
};