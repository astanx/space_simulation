#pragma once

#include "physics/positionSource.h"
#include "physics/radiusSource.h"
#include "physics/orientationSource.h"

class TransformSource : public PositionSource, public RadiusSource, public OrientationSource
{
public:
  virtual ~TransformSource() = default;
};