#pragma once

#include "physics/source/positionSource.h"
#include "physics/source/radiusSource.h"
#include "physics/source/orientationSource.h"

class TransformSource : public PositionSource, public RadiusSource, public OrientationSource
{
public:
  virtual ~TransformSource() = default;
};