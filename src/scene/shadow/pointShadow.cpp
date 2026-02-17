#include "scene/shadow/pointShadow.h"

#include "debug/logger.h"

#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedTexture.h"

#include "graphics/shader.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void PointShadow::init()
{
  this->shadowMapTexture->bind();

  for (unsigned i = 0; i < 6; i++)
    GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                         this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  {
    ScopedFramebuffer shadowMap(*this->shadowMapFBO, GL_FRAMEBUFFER);

    this->shadowMapFBO->attachTexture(GL_DEPTH_ATTACHMENT, this->shadowMapTexture->getId(), 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    this->shadowMapFBO->checkComplete();
  }
};

// Constructor
PointShadow::PointShadow(const GLuint width, const GLuint height, glm::vec3 lightPos, float nearPlane, float farPlane) : Shadow(width, height)
{
  this->lightPos = lightPos;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;
  this->aspectRatio = static_cast<float>(width) / static_cast<float>(height);

  this->shadowMapTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);

  this->init();
};

// Public functions
void PointShadow::bind(Shader &shader, int textureUnit) const
{
  this->shadowMapTexture->activate(textureUnit);
  this->shadowMapTexture->bind();
  shader.set1i(textureUnit, "depthMap");
}