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

// Public functions
void Star::drift(double dt)
{
  this->position += this->velocity * dt;
  // this->renderPosition = this->realToVisualPos(this->position);
}

void Star::halfKick(const std::vector<Object *> &bodies, double dt)
{
  this->acceleration = glm::dvec3(0.0);

  for (Object *other : bodies)
  {
    if (other == this)
      continue;

    this->applyGravitation(*other);
  }

  this->velocity += dt * this->acceleration; // kick
}