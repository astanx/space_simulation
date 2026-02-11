#pragma once

#include <functional>

#include <glm/glm.hpp>

glm::dvec3 rk4_order(const glm::dvec3 &y, double t,
                     double dt, const std::function<glm::dvec3(const glm::dvec3 &, double)> &f);