#include "graphics/skybox.h"

#include "debug/logger.h"

#include "resources/resources.h"
#include "resources/resourceManager.h"

#include "render/renderConfig.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "graphics/primitives/cube.h"

#include "graphics/buffers/renderBuffer.h"

#include "graphics/framebuffers/framebuffer.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedCullFace.h"
#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedDepthFunc.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedShader.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedBuffer.h"

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
    free(image);
    return;
  }

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

  this->convertMapToCubemap(resourceManager, captureProjection, captureViews);
  this->convertMapToIrradiance(resourceManager, captureProjection, captureViews);
}
void Skybox::convertMapToCubemap(ResourceManager &resourceManager, glm::mat4 captureProjection, glm::mat4 captureViews[])
{
  Framebuffer captureFBO;
  RenderBuffer captureRBO;

  Shader &cubemapShader = resourceManager.GetShader(Res::CUBEMAP_SHADER);
  ScopedShader cubemap(cubemapShader);

  ScopedTexture env(*this->environmentTexture, TextureBindingPoints::EquirectangularMap);
  ScopedViewport view(0, 0, this->width, this->height);

  cubemapShader.set1i(TextureBindingPoints::EquirectangularMap, "equirectangularMap");
  cubemapShader.setMat4fv(captureProjection, "ProjectionMatrix");

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

    this->mesh.render();
  }

  {
    ScopedTexture cubemapText(*this->cubemapTexture);
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
  }
}
void Skybox::convertMapToIrradiance(ResourceManager &resourceManager, glm::mat4 captureProjection, glm::mat4 captureViews[])
{
  Framebuffer captureFBO;
  RenderBuffer captureRBO;
  int irradianceWidth = 32;
  int irradianceHeight = 32;

  Shader &convolutionShader = resourceManager.GetShader(Res::CONVOLUTION_SHADER);

  ScopedFramebuffer captures(captureFBO, GL_FRAMEBUFFER);

  captureRBO.bind(GL_RENDERBUFFER);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceWidth, irradianceHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO.getId());
  captureFBO.checkComplete();

  ScopedShader convolution(convolutionShader);
  ScopedTexture enva(*this->cubemapTexture, TextureBindingPoints::EnvironmentMap);

  convolutionShader.set1i(TextureBindingPoints::EnvironmentMap, "environmentMap");
  convolutionShader.setMat4fv(captureProjection, "ProjectionMatrix");

  ScopedViewport viewp(0, 0, irradianceWidth, irradianceHeight);
  ScopedCullFace fronts(GL_FRONT);
  for (unsigned int i = 0; i < 6; ++i)
  {
    convolutionShader.setMat4fv(captureViews[i], "ViewMatrix");
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMap->getId(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->mesh.render();
  }

  {
    ScopedTexture irradiance(*this->irradianceMap);
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
  }
}

void Skybox::initIrradianceMap(GLint internalFormat, GLenum format)
{
  this->irradianceMap = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);

  this->irradianceMap->bind();

  for (unsigned int i = 0; i < 6; ++i)
    GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, 32, 32, 0,
                         format, GL_FLOAT, nullptr));

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

// Constructor/Destructor
Skybox::Skybox(std::vector<std::string> &faces) : mesh(std::make_unique<Cube>(), VertexLayout::PositionOnly)
{
  if (faces.size() != 6)
    Logger::logFatal("Skybox", "Constructor requires 6 faces");

  GLint internalFormat = GL_RGBA8;
  GLenum format = GL_RGBA;

  this->createCubemap();
  this->allocateCubemapFaces(internalFormat, format, GL_UNSIGNED_BYTE);
  this->loadCubemap(faces, internalFormat, format, GL_UNSIGNED_BYTE);
}

Skybox::Skybox(std::string environmentMap, ResourceManager &resourceManager) : mesh(std::make_unique<Cube>(), VertexLayout::PositionOnly)
{
  GLint internalFormat = GL_RGBA32F;
  GLenum format = GL_RGBA;

  this->loadEnvironmentMap(environmentMap, internalFormat, format);

  this->createCubemap();
  this->allocateCubemapFaces(internalFormat, format, GL_FLOAT);

  this->initIrradianceMap(internalFormat, format);

  this->loadCubemapFromMap(resourceManager);
}

// Public functions
void Skybox::render(Shader &shader) const
{
  ScopedDepthFunc depthLequal(RendererConfig::Depth::SkyboxFunc);
  ScopedDepthMask depthMask(GL_FALSE);
  ScopedTexture skyboxText(*this->cubemapTexture, TextureBindingPoints::Skybox);

  shader.set1i(TextureBindingPoints::Skybox, "skybox");

  this->mesh.render();
}

void Skybox::bindIrradianceMap(Shader &shader) const
{
  this->irradianceMap->activate(TextureBindingPoints::IrradianceMap);
  this->irradianceMap->bind();

  shader.set1i(TextureBindingPoints::IrradianceMap, "irradianceMap");
}
void Skybox::unbindIrradianceMap() const
{
  this->irradianceMap->unbind(TextureBindingPoints::IrradianceMap);
}