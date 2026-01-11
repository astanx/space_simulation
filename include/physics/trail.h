#pragma once

#include <vector>
#include <glm/glm.hpp>

class Shader;
class Mesh;

class Trail
{
private:
  std::unique_ptr<Mesh> trail;
  void generateTrail(const std::vector<glm::dvec3> &trailVec);

public:
  Trail(const std::vector<glm::dvec3> &trailVec);
  ~Trail() = default;

  void render();
};