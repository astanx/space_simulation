#pragma once

struct FrameContext
{
  float width;
  float height;

  glm::dvec3 camPosition;

  float aspect;
  float exposure;
};