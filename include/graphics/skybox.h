#pragma once

#include "graphics/mesh.h"
#include "graphics/texture.h"

#include <vector>
#include <GL/glew.h>

class Shader;
class ResourceManager;

class Skybox
{
private:
  std::unique_ptr<Texture> cubemapTexture;
  std::unique_ptr<Texture> environmentTexture;
  int width;
  int height;

  Mesh mesh;

  void createCubemap();
  void allocateCubemapFaces(GLint internalFormat, GLenum format, GLenum type);
  void loadCubemap(std::vector<std::string> faces, GLint internalFormat, GLenum format, GLenum type);

  void loadCubemapFromMap(ResourceManager &resourceManager);
  void loadEnvironmentMap(std::string environmentMap, GLint internalFormat, GLenum format);

public:
  Skybox(std::vector<std::string> &faces);
  Skybox(std::string environmentMap, ResourceManager& resourceManager);
  ~Skybox() = default;

  void render(Shader &shader) const;
};