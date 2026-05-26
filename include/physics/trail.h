#pragma once

#include "graphics/mesh.h"

#include <vector>
#include <glm/glm.hpp>

class Shader;
class Camera;

class Trail
{
private:
  std::unique_ptr<Mesh> trail;
  std::vector<glm::dvec3> trailVec;
  void generateTrail(const Camera &camera);

public:
  Trail(const std::vector<glm::dvec3> &trailVec);
  ~Trail() = default;

  void render() const;
  void update(const Camera &camera);
};