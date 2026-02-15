#include "graphics/state/scopedViewport.h"

ScopedViewport::ScopedViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  glGetIntegerv(GL_VIEWPORT, this->prevViewport);

  glViewport(x, y, width, height);
};

ScopedViewport::~ScopedViewport()
{
  glViewport(this->prevViewport[0], this->prevViewport[1], this->prevViewport[2], this->prevViewport[3]);
}