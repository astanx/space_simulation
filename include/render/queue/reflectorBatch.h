#pragma once

#include "resources/range.h"
#include "render/queue/renderBatch.h"

#include <vector>

class ReflectanceAcceptor;

struct ReflectorBatch
{
  ReflectanceAcceptor *acceptor;
  Range acceptorRange;

  RenderBatch reflector;
};