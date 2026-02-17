#pragma once

#include "graphics/mesh.h"
#include "graphics/texture.h"

#include <vector>
#include <GL/glew.h>

class Shader;

class Skybox
{
private:
  std::unique_ptr<Texture> texture;
  int width;
  int height;

  Mesh mesh;

  void loadCubemap(std::vector<const char *> faces);

public:
  Skybox(std::vector<const char *> &faces);
  ~Skybox() = default;

  void render(Shader &shader) const;
};