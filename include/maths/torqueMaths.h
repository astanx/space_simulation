#pragma once

#include "physics/object.h"
#include "maths/momentsMaths.h"
#include "maths/torqueMathsFormulas.h"

#include <glm/glm.hpp>

namespace TorqueMaths
{
  template <typename real>
  glm::vec<3, real> calculateGravitationalTorque(Object *object, const Object *body)
  {
    glm::vec<3, real> dp = body->getPosition() - object->getPosition();
    real d = glm::length(dp);
    return ::calculateGravitationalTorque<real>(dp, d, object->getQuadrupoleTensor(), body->getMu());
  }
  template <typename real>
  glm::vec<3, real> calculateTidalTorque(Object *object, const Object *body)
  {
    const TidalParameters &p = object->getTidalParameters();
    if (p.k2 == -1 || p.Q == -1)
      return glm::vec<3, real>(0.0);

    real mu = body->getMu();
    glm::vec<3, real> dp = object->getPosition() - body->getPosition();
    real d = glm::length(dp);

    return ::calculateTidalTorque<real>(dp, d, object->getAngularVelocity(), object->getVelocity(), object->getRadius(), p.k2, p.Q, body->getVelocity(), body->getMu());
  }

  template <typename real>
  glm::dvec3 calculateTorque(Object *object, const std::vector<Object *> &bodies)
  {
    glm::dvec3 torque = glm::dvec3(0);
    for (const Object *body : bodies)
    {
      if (body == object)
        continue;

      torque += TorqueMaths::calculateGravitationalTorque<real>(object, body);
      torque += TorqueMaths::calculateTidalTorque<real>(object, body);
    }
    return torque;
  }
};