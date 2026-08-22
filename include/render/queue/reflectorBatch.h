#pragma once

#include "resources/range.h"
#include "render/queue/renderBatch.h"

#include <vector>

class ReflectanceAcceptor;

struct ReflectorBatch
{
  const ReflectanceAcceptor *acceptor;
  Range acceptorRange;

  RenderBatch reflector;
};