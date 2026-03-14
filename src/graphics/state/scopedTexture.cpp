#include "graphics/state/scopedTexture.h"

#include "debug/logger.h"

GLenum ScopedTexture::getBindingEnumForTarget(GLenum target)
{
  switch (target)
  {
  case GL_TEXTURE_1D:
    return GL_TEXTURE_BINDING_1D;
  case GL_TEXTURE_2D:
    return GL_TEXTURE_BINDING_2D;
  case GL_TEXTURE_3D:
    return GL_TEXTURE_BINDING_3D;
  case GL_TEXTURE_1D_ARRAY:
    return GL_TEXTURE_BINDING_1D_ARRAY;
  case GL_TEXTURE_2D_ARRAY:
    return GL_TEXTURE_BINDING_2D_ARRAY;
  case GL_TEXTURE_CUBE_MAP:
    return GL_TEXTURE_BINDING_CUBE_MAP;
  case GL_TEXTURE_CUBE_MAP_ARRAY:
    return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
  case GL_TEXTURE_RECTANGLE:
    return GL_TEXTURE_BINDING_RECTANGLE;
  case GL_TEXTURE_BUFFER:
    return GL_TEXTURE_BINDING_BUFFER;
  default:
    Logger::logError("Scoped texture", "Can not find binding enum for target - " + std::to_string(target));
    return GL_TEXTURE_BINDING_2D;
  }
}

ScopedTexture::ScopedTexture(const Texture &texture, GLint unit, bool saveState)
{
  this->unit = unit;
  this->target = texture.getTarget();

  if (saveState)
    glGetIntegerv(GL_ACTIVE_TEXTURE, &this->prevActiveTexture);

  if (unit != -1)
    texture.activate(unit);

  if (saveState)
  {
    GLenum bindingEnum = this->getBindingEnumForTarget(this->target);
    glGetIntegerv(bindingEnum, &this->prevTexture);
  }
  texture.bind();
};

ScopedTexture::ScopedTexture(GLuint id, GLenum target, GLint unit, bool saveState)
{
  this->unit = unit;
  this->target = target;

  if (saveState)
    glGetIntegerv(GL_ACTIVE_TEXTURE, &this->prevActiveTexture);

  if (unit != -1)
    glActiveTexture(GL_TEXTURE0 + unit);

  if (saveState)
  {
    GLenum bindingEnum = this->getBindingEnumForTarget(this->target);
    glGetIntegerv(bindingEnum, &this->prevTexture);
  }

  glBindTexture(this->target, id);
};

ScopedTexture::~ScopedTexture()
{
  if (unit != -1)
    glActiveTexture(GL_TEXTURE0 + this->unit);
  
  glBindTexture(this->target, this->prevTexture);

  glActiveTexture(this->prevActiveTexture);
}