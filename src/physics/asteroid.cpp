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
glm::mat4 Asteroid::getModelMatrix()
{
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(this->renderPosition));
  return model;
};
