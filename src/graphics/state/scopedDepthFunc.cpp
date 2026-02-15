#include "graphics/state/scopedDepthFunc.h"

ScopedDepthFunc::ScopedDepthFunc(GLint func)
{
  glGetIntegerv(GL_DEPTH_FUNC, &this->prevFunc);

  glDepthFunc(func);
};

ScopedDepthFunc::~ScopedDepthFunc()
{
  glDepthFunc(this->prevFunc);
}