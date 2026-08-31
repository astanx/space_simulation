#pragma once

#include "graphics/vertex.h"

#include <vector>
#include <string>

std::vector<VertexPositionTexcoordNormalColor> loadOBJmodel(const std::string &filename);