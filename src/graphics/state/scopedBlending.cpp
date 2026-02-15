#include "graphics/state/scopedBlending.h"

ScopedBlending::ScopedBlending(bool enable,
                               GLenum srcFactor,
                               GLenum dstFactor)
{
  glGetBooleanv(GL_BLEND, &this->wasEnabled);
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &this->oldSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &this->oldDst);

  if (enable)
  {
    glEnable(GL_BLEND);
    glBlendFunc(srcFactor, dstFactor);
  }
  else
    glDisable(GL_BLEND);
};

ScopedBlending::~ScopedBlending()
{
  if (this->wasEnabled)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
  glBlendFunc(this->oldSrc, this->oldDst);
}