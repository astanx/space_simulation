#pragma once

#include "resources/range.h"
#include "render/renderFlags.h"

class Model;

struct RenderBatch
{
  const Model *model;
  Range range;
  RenderFlags flag;
};