#pragma once

#include "resources/data/range.h"
#include "render/state/renderFlags.h"

class Model;

struct RenderBatch
{
  const Model *model;
  Range range;
  RenderFlags flag;
};