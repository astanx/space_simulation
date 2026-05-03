#include "graphics/state/scopedPolygonOffset.h"

ScopedPolygonOffset::ScopedPolygonOffset(bool enable, float factor, float units)
{
  glGetBooleanv(GL_POLYGON_OFFSET_FILL, &this->wasEnabled);

  if (enable)
  {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(factor, units);
  }
  else
    glDisable(GL_POLYGON_OFFSET_FILL);
};

ScopedPolygonOffset::~ScopedPolygonOffset()
{
  if (this->wasEnabled)
    glEnable(GL_POLYGON_OFFSET_FILL);
  else
    glDisable(GL_POLYGON_OFFSET_FILL);
}