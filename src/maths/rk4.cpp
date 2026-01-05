#include "maths/rk4.h"

// MAY BE REWORK ( DOES NOT WORK FOR BIG TIM_SCALE )

glm::dvec3 rk4_order(const glm::dvec3& y, double t, 
  double dt, const std::function<glm::dvec3(const glm::dvec3&, double)>& f)
  {
    glm::dvec3 k1 = dt * f(y, t);
    glm::dvec3 k2 = dt * f(y + k1 * 0.5, t + dt * 0.5);
    glm::dvec3 k3 = dt * f(y + k2 * 0.5, t + dt * 0.5);
    glm::dvec3 k4 = dt * f(y + k3, t + dt);

    glm::dvec3 y2 = y + (k1 + k2 * 2.0 + k3 * 2.0 + k4) / 6.0;
    
    return y2;
  }