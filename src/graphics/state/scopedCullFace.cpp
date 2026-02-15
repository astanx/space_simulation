#include "graphics/state/scopedCullFace.h"

ScopedCullFace::ScopedCullFace(GLenum face)
{
  glGetIntegerv(GL_CULL_FACE_MODE, &this->prevFace);
  glGetBooleanv(GL_CULL_FACE, &this->prevEnabled);

  glCullFace(face);
  glEnable(GL_CULL_FACE);
};

ScopedCullFace::~ScopedCullFace()
{
  glCullFace(this->prevFace);
  if (this->prevEnabled)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
}