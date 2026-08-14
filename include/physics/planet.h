#pragma once

#include "physics/orbitalObject.h"
#include "physics/moon.h"
#include "physics/atmosphere.h"

#include "render/modelSource.h"

#include "graphics/framebuffers/framebuffer.h"
#include "graphics/buffers/renderBuffer.h"

class Camera;

struct KeplerElements;

class Planet : public OrbitalObject, public ModelSource
{
protected:
  std::vector<std::unique_ptr<Moon>> moons;

  double g; // Acceleration of free fall of the Planet

  std::unique_ptr<Atmosphere> atmosphere;

public:
  Planet(Object *centralBody, double mu, Radii radii, const KeplerElements &keplerElements, TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField(), double g = 0.0);
  ~Planet();

  void addMoon(std::unique_ptr<Moon> moon);
  void addAtmosphere(std::unique_ptr<Atmosphere> atmosphere);

  double getFreeFallAcc() const;
};