#pragma once

#include <GL/glew.h>

class Texture
{
private:
  GLuint id;
  int width;
  int height;
  unsigned int type;

public:
  Texture(const char *fileName, GLenum type);

  ~Texture();

  inline GLuint getId() const { return this->id; };

  void bind(const GLint textureUnit);

  void unbind();

  void loadFromFile(const char *fileName);
};