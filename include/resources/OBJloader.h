#pragma once

#include "graphics/vertex.h"

#include <vector>

std::vector<VertexPositionTexcoordNormalColor> loadOBJmodel(const std::string &filename);