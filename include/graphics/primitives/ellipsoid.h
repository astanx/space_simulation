#pragma once

#include "graphics/primitives/primitives.h"

struct Radii;

class Ellipsoid : public Primitive
{
protected:
  void computeTangents(std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);

public:
  Ellipsoid(unsigned segments, Radii radii, bool tangent = false);
};