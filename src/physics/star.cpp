#include <physics/star.h>

#include <physics/constants.h>

#include <graphics/model.h>

// Constructor
Star::Star(double mu, double radius, double luminosity, glm::dvec3 position, glm::dvec3 velocity)
    : Object(mu / G, radius, position, velocity), ModelSource(static_cast<const PositionSource &>(*this), radius * VISUAL_RADIUS_SCALE)
{
  this->mu = mu;
  this->luminosity = luminosity;
}