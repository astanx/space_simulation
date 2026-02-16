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
  Texture(GLenum target);

  ~Texture();

  inline GLuint getId() const { return this->id; };
  inline GLenum getTarget() const { return this->target; };

  void bind() const;
  void unbind(const GLint textureUnit) const;

  void activate(const GLint textureUnit) const;

  void loadFromFile(const std::string &fileName);
};