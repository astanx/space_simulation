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

  uint radianceSize = 32;

  std::unique_ptr<Texture> moonRadianceTexture;
  std::unique_ptr<Framebuffer> moonRadianceFBO;

  std::unique_ptr<RenderBuffer> moonRBO;

  std::unique_ptr<Atmosphere> atmosphere;

  void initMoonRadianceTexture();
  void initMoonRadianceFBO();

public:
  Planet(Object *centralBody, double mu, Radii radii, const KeplerElements &keplerElements, TidalParameters tidalParameters = TidalParameters(), GravityField gravityField = GravityField(), double g = 0.0);
  ~Planet();

  void addMoon(std::unique_ptr<Moon> moon);
  void addAtmosphere(std::unique_ptr<Atmosphere> atmosphere);

  double getFreeFallAcc() const;

  void render(Shader &shader, Frustum *frustum, bool force = false) const override;

  void renderAtmosphere(Shader &shader, Frustum *frustum, bool force = false) const;
  void renderMoonsRadiance(Shader &shader, const Camera &camera) const;
};