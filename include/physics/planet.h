#pragma once

#include "physics/orbitalObject.h"
#include "physics/moon.h"

#include "render/modelSource.h"

#include "graphics/framebuffers/framebuffer.h"
#include "graphics/buffers/renderBuffer.h"

class Model;
class Camera;

struct KeplerElements;

class Planet : public OrbitalObject, public ModelSource
{
protected:
  std::vector<std::unique_ptr<Moon>> moons;

  uint radianceSize = 32;

  std::unique_ptr<Texture> moonRadianceTexture;
  std::unique_ptr<Framebuffer> moonRadianceFBO;

  std::unique_ptr<RenderBuffer> moonRBO;

  void initMoonRadianceTexture();
  void initMoonRadianceFBO();

public:
  Planet(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements);
  ~Planet() = default;

  void addMoon(std::unique_ptr<Moon> moon);

  void render(Shader &shader, Frustum* frustum, bool force = false) const override;

  void renderMoonsRadiance(Shader &shader, const Camera& camera) const;
};