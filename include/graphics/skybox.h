#pragma once

#include "graphics/mesh.h"

#include <vector>
#include <GL/glew.h>

class Shader;

class Skybox
{
private:
  GLuint id;
  int width;
  int height;

  Mesh mesh;

  void loadCubemap(std::vector<const char *> faces);

public:
  Skybox(std::vector<const char *> &faces);
  ~Skybox();

  void render(Shader &shader) const;

  void bind(const GLint textureUnit) const;
  void unbind(const GLint textureUnit) const;
};