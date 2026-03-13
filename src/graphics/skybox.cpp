#include "graphics/skybox.h"

#include "debug/logger.h"

#include "resources/resources.h"
#include "resources/resourceManager.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "graphics/primitives/cube.h"

#include "graphics/buffers/renderBuffer.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedCullFace.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthFunc.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedShader.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedBuffer.h"

#include "graphics/primitives/cube.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#define TINYEXR_IMPLEMENTATION
#include "external/tinyexr.h"
#include "external/stb_image.h"

#include <iostream>

// Private functions
void Skybox::allocateCubemapFaces(GLint internalFormat, GLenum format, GLenum type)
{
  this->cubemapTexture->bind();

  for (int i = 0; i < 6; ++i)
    GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         internalFormat,
                         this->width,
                         this->height,
                         0,
                         format,
                         type,
                         nullptr));

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::createCubemap()
{
  this->cubemapTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);
  this->cubemapTexture->bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::loadCubemap(std::vector<std::string> faces, GLint internalFormat, GLenum format, GLenum type)
{
  this->cubemapTexture->bind();

  for (unsigned int i = 0; i < faces.size(); i++)
  {
    int channels;
    unsigned char *image = stbi_load(faces[i].c_str(), &this->width, &this->height, &channels, 4);
    if (image)
      GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                           0, internalFormat, this->width, this->height, 0, format, type, image));
    else
      Logger::logError("Skybox", "Cubemap texture failed to load at path - " + faces[i]);

    stbi_image_free(image);
  }

  GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
};

void Skybox::loadEnvironmentMap(std::string environmentMap, GLint internalFormat, GLenum format)
{
  int width, height;
  float *image = nullptr;
  const char *err = nullptr;

  int ret = LoadEXR(&image, &width, &height, environmentMap.c_str(), &err);

  if (ret != TINYEXR_SUCCESS)
  {
    Logger::logError("Skybox", "Failed to load .exr: " + std::string(err));
    FreeEXRErrorMessage(err);
  }
  else

    if (image)
      this->environmentTexture = std::make_unique<Texture>(width, height, GL_TEXTURE_2D, internalFormat, format, GL_FLOAT, image);
  else
    Logger::logError("Skybox", "Failed to load environment map at path " + environmentMap);

  free(image);

  glBindTexture(GL_TEXTURE_2D, 0);

  this->width = height;
  this->height = height;
}

void Skybox::loadCubemapFromMap(ResourceManager &resourceManager)
{
  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

  glm::mat4 captureViews[] =
      {
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

  Shader &cubemapShader = resourceManager.GetShader(Res::CUBEMAP_SHADER);
  ScopedShader cubemap(cubemapShader);

  ScopedTexture env(*this->environmentTexture, TextureBindingPoints::EnvironmentMap);
  ScopedViewport view(0, 0, this->width, this->height);

  cubemapShader.set1i(TextureBindingPoints::EnvironmentMap, "equirectangularMap");
  cubemapShader.setMat4fv(captureProjection, "ProjectionMatrix");

  Framebuffer captureFBO;
  RenderBuffer captureRBO;

  ScopedFramebuffer capture(captureFBO, GL_FRAMEBUFFER);

  captureRBO.bind(GL_RENDERBUFFER);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->width, this->height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO.getId());
  captureFBO.checkComplete();

  ScopedCullFace front(GL_FRONT);
  for (unsigned int i = 0; i < 6; ++i)
  {
    cubemapShader.setMat4fv(captureViews[i], "ViewMatrix");
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->cubemapTexture->getId(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mesh.render();
  }

  ScopedTexture cubemapText(*this->cubemapTexture, GL_TEXTURE_CUBE_MAP);
  GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
}

// Constructor/Destructor
Skybox::Skybox(std::vector<std::string> &faces) : mesh(std::make_unique<Cube>(), VertexLayout::PositionOnly)
{
  if (faces.size() != 6)
    Logger::logFatal("Skybox", "Constructor requires 6 faces");

  this->createCubemap();
  this->allocateCubemapFaces(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
  this->loadCubemap(faces, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
}

Skybox::Skybox(std::string environmentMap, ResourceManager &resourceManager) : mesh(std::make_unique<Cube>(), VertexLayout::PositionOnly)
{
  this->loadEnvironmentMap(environmentMap, GL_RGBA32F, GL_RGBA);

  this->createCubemap();
  this->allocateCubemapFaces(GL_RGBA32F, GL_RGBA, GL_FLOAT);

  this->loadCubemapFromMap(resourceManager);
}

// Public functions
void Skybox::render(Shader &shader) const
{
  ScopedDepthFunc depthLequal(GL_LEQUAL);
  ScopedDepthMask depthMask(GL_FALSE);
  ScopedTexture skyboxText(*this->cubemapTexture, TextureBindingPoints::Skybox);

  shader.set1i(TextureBindingPoints::Skybox, "skybox");

  this->mesh.render();
}