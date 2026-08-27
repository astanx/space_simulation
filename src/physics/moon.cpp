#include "physics/moon.h"

#include "physics/constants.h"

#include "graphics/vertex.h"
#include "graphics/shader.h"

#include <iostream>

// Constructor
Moon::Moon(OrbitalObject *centralBody, double mu, Radii radii, const KeplerElements<double> &keplerElements, TidalParameters tidalParameters, GravityField gravityField)
    : OrbitalObject(centralBody, mu, radii, keplerElements, tidalParameters, gravityField, false), ModelSource(static_cast<const TransformSource &>(*this), radii.mean)
{
  if (this->useTrail)
    this->generateTrail();
}