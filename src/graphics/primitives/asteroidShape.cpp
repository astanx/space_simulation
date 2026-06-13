#include "graphics/primitives/asteroidShape.h"

#include "graphics/vertex.h"

#include "maths/factorial.h"

#include <vector>
#include <cmath>
#include <complex>

// Private function
double AsteroidShape::calculateR(double angle, double m, double a, double b, double n1, double n2, double n3)
{
  double term1 = std::pow(std::fabs(std::cos(m * angle / 4) / a), n2);
  double term2 = std::pow(std::fabs(std::sin(m * angle / 4) / b), n3);
  return std::pow(term1 + term2, -1.0 / n1);
}

// Constructor
AsteroidShape::AsteroidShape(double thetaSteps, double phiSteps, double m, double a, double b, double n1, double n2, double n3, double scale)
    : Primitive()
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  for (size_t ip = 0; ip <= size_t(phiSteps); ++ip)
  {
    double phi = M_PI * ip / phiSteps;
    double r2 = calculateR(phi, m, a, b, n1, n2, n3);

    for (size_t it = 0; it <= size_t(thetaSteps); ++it)
    {
      double theta = 2.0 * M_PI * it / thetaSteps;
      double r1 = calculateR(theta, m, a, b, n1, n2, n3);
      double r = r1 * r2;

      double x = r * std::sin(phi) * std::cos(theta);
      double y = r * std::sin(phi) * std::sin(theta);
      double z = r * std::cos(phi);

      glm::vec3 pos(x, y, z);
      pos *= scale;

      glm::vec3 normal = glm::normalize(pos);

      vertices.push_back(Vertex{
          pos,
          glm::vec3(1.0f, 1.0f, 1.0f),
          glm::vec2(theta / (2.0f * M_PI), phi / M_PI),
          normal});
    }
  }

  for (size_t ip = 0; ip < size_t(phiSteps); ++ip)
  {
    for (size_t it = 0; it < size_t(thetaSteps); ++it)
    {
      size_t i0 = ip * (thetaSteps + 1) + it;
      size_t i1 = i0 + 1;
      size_t i2 = (ip + 1) * (thetaSteps + 1) + it;
      size_t i3 = i2 + 1;

      indices.push_back(i0);
      indices.push_back(i2);
      indices.push_back(i1);

      indices.push_back(i1);
      indices.push_back(i2);
      indices.push_back(i3);
    }
  }

  this->computeTangents(vertices, indices);

  this->radii.polar = calculateR(0, m, a, b, n1, n2, n3);
  this->radii.equatorian = calculateR(M_PI / 2, m, a, b, n1, n2, n3);
  this->radii.mean = (this->radii.polar + this->radii.equatorian) / 2;

  this->set(vertices, indices);
}
