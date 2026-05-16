#pragma once

#include "graphics/mesh.h"

#include <vector>
#include <glm/glm.hpp>

class Shader;

class Trail
{
private:
  std::unique_ptr<Mesh> trail;
  std::vector<glm::dvec3> trailVec;
  void generateTrail(const glm::dvec3 &camPosition = glm::dvec3(0.0));

public:
  Trail(const std::vector<glm::dvec3> &trailVec);
  ~Trail() = default;

  void render() const;
  void update(const glm::dvec3 &camPosition);
};