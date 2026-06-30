#pragma once

#include <glm/glm.hpp>
#include <array>
#include <iostream>

enum FaceSide
{
  LEFT_FACE = 0,
  RIGHT_FACE,
  TOP_FACE,
  BOTTOM_FACE,
  NEAR_FACE,
  FAR_FACE
};

struct Frustum
{
  std::array<glm::vec4, 6> faces;

  bool isVisibleSphere(glm::vec3 center, float radius)
  {
    for (auto &face : faces)
    {
      float d = dot(glm::vec3(face), center) + face.w;
      if (d < -radius)
        return false;
    }
    return true;
  }

  static bool shouldBeProcessed(Frustum *frustum, glm::vec3 center, float radius, bool force)
  {
    if (!force && frustum && !frustum->isVisibleSphere(center, radius))
      return false;

    return true;
  }
};