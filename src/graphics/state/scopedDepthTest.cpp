#include "graphics/state/scopedDepthTest.h"

ScopedDepthTest::ScopedDepthTest(bool enable)
{
  glGetBooleanv(GL_DEPTH_TEST, &this->wasEnabled);

  if (enable)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
};

ScopedDepthTest::~ScopedDepthTest()
{
  if (this->wasEnabled)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}