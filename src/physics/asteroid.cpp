#include "physics/asteroid.h"

#include "physics/constants.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Constructor
Asteroid::Asteroid(Object *centralBody, double mu, Radii radii, const KeplerElements &elements)
    : OrbitalObject(centralBody, mu, radii, elements)
{
}
