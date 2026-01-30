#include "scene/shadow/pointShadow.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

// Private functions
void PointShadow::init()
{
  glGenFramebuffers(1, &this->shadowMapFBO);
  glGenTextures(1, &this->shadowMapTexture);

  glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadowMapTexture);
  for (unsigned i = 0; i < 6; i++)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowMapTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cerr << "ERROR::POINT_SHADOW::FRAMEBUFFER_NOT_COMPLETE" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

// Constructor
PointShadow::PointShadow(const GLuint width, const GLuint height, glm::vec3 lightPos, float nearPlane, float farPlane) : Shadow(width, height)
{
  this->lightPos = lightPos;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;
  this->aspectRatio = 1.f;
  this->init();
};

// Public functions
void PointShadow::bindShadowMapFBO() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowMapTexture, 0);
  // glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);
}

void PointShadow::bind(Shader &shader, int textureUnit) const
{
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadowMapTexture);
  shader.set1i(textureUnit, "depthMap");
}