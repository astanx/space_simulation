#include "physics/asteroid.h"

#include "physics/constants.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Constructor
Asteroid::Asteroid(Object *centralBody, double mu, double radius, const KeplerElements &elements)
    : OrbitalObject(centralBody, mu, radius, elements), ModelSource(static_cast<PositionSource *>(this), radius * VISUAL_RADIUS_SCALE)
{
}

// Public functions
glm::dmat4 Asteroid::getModelMatrix()
{
  glm::dmat4 model(1.0f);
  model = glm::translate(model, this->renderPosition);
  return model;
};
