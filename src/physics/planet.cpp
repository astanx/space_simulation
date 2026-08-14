#include "physics/planet.h"

#include "debug/logger.h"

#include "camera/camera.h"

#include "physics/object.h"
#include "physics/constants.h"
#include "physics/orbit.h"

#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/vertex.h"
#include "graphics/texture.h"

#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedBlending.h"

#include "graphics/bindings/texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Constructor / Destructor
Planet::Planet(Object *centralBody, double mu, Radii radii, const KeplerElements &keplerElements, TidalParameters tidalParameters, GravityField gravityField, double g)
    : OrbitalObject(centralBody, mu, radii, keplerElements, tidalParameters, gravityField), ModelSource(static_cast<const TransformSource &>(*this), radii.mean)
{
  this->g = g;
}

Planet::~Planet() = default;

// Public functions
// void Planet::renderAtmosphere(Shader &shader) const
// {
//   if (this->atmosphere)
//   {
//     this->atmosphere->bindTextures();
//     shader.set1f(this->g, "g");
//     this->atmosphere->sendToShader(shader);
//     ModelSource::renderLayers(shader);
//     this->atmosphere->unbindTextures();
//   }
// }

void Planet::addMoon(std::unique_ptr<Moon> moon)
{
  this->moons.push_back(std::move(moon));
}

void Planet::addAtmosphere(std::unique_ptr<Atmosphere> atmosphere)
{
  this->atmosphere = std::move(atmosphere);
}

double Planet::getFreeFallAcc() const
{
  return this->g;
}