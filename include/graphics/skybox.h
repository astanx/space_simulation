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
  std::unique_ptr<Texture> irradianceMap;

  int width;
  int height;

  Mesh mesh;

  void createCubemap();
  void allocateCubemapFaces(GLint internalFormat, GLenum format, GLenum type);
  void loadCubemap(std::vector<std::string> faces, GLint internalFormat, GLenum format, GLenum type);

  void initIrradianceMap(GLint internalFormat, GLenum format);
  void convertMapToIrradiance(ResourceManager &resourceManager, glm::mat4 captureProjection, glm::mat4 captureViews[]);
  void convertMapToCubemap(ResourceManager &resourceManager, glm::mat4 captureProjection, glm::mat4 captureViews[]);
  void loadCubemapFromMap(ResourceManager &resourceManager);
  void loadEnvironmentMap(std::string environmentMap, GLint internalFormat, GLenum format);

public:
  Skybox(std::vector<std::string> &faces);
  Skybox(std::string environmentMap, ResourceManager &resourceManager);
  ~Skybox() = default;

  void render(Shader &shader) const;

  void bindIrradianceMap(Shader &shader) const;
  void unbindIrradianceMap() const;
};