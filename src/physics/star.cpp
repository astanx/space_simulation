#include <physics/star.h>

#include <physics/constants.h>

#include <graphics/model.h>

// Constructor
Star::Star(double mu, Radii radii, double luminosity, glm::dvec3 position, glm::dvec3 velocity, TidalParameters tidalParameters, GravityField gravityField)
    : Object(mu / G, radii, tidalParameters, gravityField, position, velocity), ModelSource(static_cast<const PositionSource &>(*this), radii.mean * VISUAL_RADIUS_SCALE)
{
  this->mu = mu;
  this->luminosity = luminosity;
}