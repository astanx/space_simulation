#pragma once

#include <GL/glew.h>

#include <string>

class Texture
{
private:
  GLuint id;
  int width;
  int height;
  unsigned int type;

public:
  Texture(const std::string &fileName, GLenum type);
  Texture(GLsizei width, GLsizei height, GLenum type, const void *pixels = nullptr);

  ~Texture();

  inline GLuint getId() const { return this->id; };

  void bind(const GLint textureUnit);

  void unbind(const GLint textureUnit);

  void loadFromFile(const std::string &fileName);
};