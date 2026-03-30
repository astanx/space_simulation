#pragma once

#include "physics/orbitalObject.h"
#include "physics/moon.h"

#include "render/renderable.h"

#include "graphics/framebuffers/framebuffer.h"

class Model;
class Camera;

struct KeplerElements;

class Planet : public OrbitalObject, public Renderable
{
protected:
  std::vector<std::unique_ptr<Moon>> moons;

  uint radianceSize = 32;

  std::unique_ptr<Texture> moonRadianceTexture;
  std::unique_ptr<Framebuffer> moonRadianceFBO;

  void initMoonRadianceTexture();
  void initMoonRadianceFBO();

public:
  Planet(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements);
  ~Planet() = default;

  void addMoon(std::unique_ptr<Moon> moon);

  void update(double dt) override;
  void render(Shader &shader) const override;
  void addModel(std::unique_ptr<Model> m) override;

  void renderMoonsRadiance(Shader &shader, const Camera& camera) const;

  void drift(double dt) override;
  void halfKick(const std::vector<Object *> &bodies, double dt) override;
};