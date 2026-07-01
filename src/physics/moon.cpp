#include "physics/moon.h"

#include "physics/constants.h"

#include "graphics/vertex.h"
#include "graphics/shader.h"

#include <iostream>

// Constructor
Moon::Moon(OrbitalObject *centralBody, double mu, Radii radii, const KeplerElements &keplerElements, const HapkeParameters &hapkeParameters, TidalParameters tidalParameters, GravityField gravityField)
    : OrbitalObject(centralBody, mu, radii, keplerElements, tidalParameters, gravityField, false), hapkeParameters(hapkeParameters), ModelSource(static_cast<const TransformSource &>(*this), radii.mean * VISUAL_RADIUS_SCALE)
{
  if (this->useTrail)
    this->generateTrail();
}

// Public functions
void Moon::sendHapkeParametersToShader(Shader &shader) const
{
  shader.set1f(this->hapkeParameters.w, "hapkeParameters.w");
  shader.set1f(this->hapkeParameters.theta, "hapkeParameters.theta");
  shader.set1f(this->hapkeParameters.h, "hapkeParameters.h");
  shader.set1f(this->hapkeParameters.b0, "hapkeParameters.b0");
  shader.set1f(this->hapkeParameters.h_cb, "hapkeParameters.h_cb");
  shader.set1f(this->hapkeParameters.b0_cb, "hapkeParameters.b0_cb");
  shader.set1f(this->hapkeParameters.b, "hapkeParameters.b");
  shader.set1f(this->hapkeParameters.c, "hapkeParameters.c");
}