#pragma once

#include "resources/range.h"
#include "render/queue/renderBatch.h"

#include <vector>

class Model;

struct ReflectorBatch
{
  Model *acceptor;
  Range range;

  std::vector<RenderBatch> reflectors;
};