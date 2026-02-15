#include "graphics/state/scopedDepthMask.h"

ScopedDepthMask::ScopedDepthMask(GLboolean mask)
{
  glGetBooleanv(GL_DEPTH_WRITEMASK, &this->prevMask);

  glDepthMask(mask);
};

ScopedDepthMask::~ScopedDepthMask()
{
  glDepthMask(this->prevMask);
}