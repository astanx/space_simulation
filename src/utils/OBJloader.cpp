#include "utils/OBJloader.h"

#include <iostream>
#include <string>
#include <fstream>

#include <sstream>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<Vertex> loadOBJmodel(const char *filename)
{
  std::vector<glm::fvec3> vertex_positions;
  std::vector<glm::fvec2> vertex_texcoords;
  std::vector<glm::fvec3> vertex_normals;

  std::vector<GLint> vertex_position_indices;
  std::vector<GLint> vertex_texcoord_indices;
  std::vector<GLint> vertex_normal_indices;

  std::vector<Vertex> vertices;

  std::ifstream inFile(filename);

  if (!inFile.is_open())
  {
    throw std::runtime_error(std::string("ERROR:OBJLOADER::Could not open file: ") + filename);
  }

  std::stringstream ss;
  std::string line = "";
  std::string prefix = "";

  // Read line-by-line
  while (std::getline(inFile, line))
  {
    // Get the prefix
    ss.clear();
    ss.str(line);
    ss >> prefix;

    // Vertices
    if (prefix == "v") // Vertex position
    {
      glm::vec3 pos;

      ss >> pos.x >> pos.y >> pos.z;
      vertex_positions.push_back(pos);
    }
    else if (prefix == "vt") // Vertex texcoord
    {
      glm::vec3 texcoord;

      ss >> texcoord.x >> texcoord.y;
      vertex_texcoords.push_back(texcoord);
    }
    else if (prefix == "vn") // Vertex normal
    {
      glm::vec3 normal;

      ss >> normal.x >> normal.y >> normal.z;
      vertex_normals.push_back(normal);
    }

    // Indices
    else if (prefix == "f")
    {
      int counter = 0;
      GLint index;
      while (ss >> index)
      {
        if (counter == 0)
          vertex_position_indices.push_back(index - 1);
        else if (counter == 1)
          vertex_texcoord_indices.push_back(index - 1);
        else if (counter == 2)
          vertex_normal_indices.push_back(index - 1);

        // Handle next
        if (ss.peek() == '/')
        {
          ++counter;
          ss.ignore(1, '/');
        }
        else if (ss.peek() == ' ')
        {
          ++counter;
          ss.ignore(1, ' ');
        }

        if (counter > 2)
          counter = 0;
      }
    }
  }

  if (vertex_positions.empty() || vertex_texcoords.empty() || vertex_normals.empty())
    std::cout << "OBJ data incomplete" << std::endl;

  vertices.resize(vertex_position_indices.size());

  for (size_t i = 0; i < vertices.size(); i++)
  {
    if (vertex_position_indices[i] < 0 || vertex_position_indices[i] >= vertex_positions.size())
      throw std::runtime_error("OBJ position index out of range");

    vertices[i].position = vertex_positions[vertex_position_indices[i]];

    if (!vertex_texcoords.empty() && i < vertex_texcoord_indices.size() && vertex_texcoord_indices[i] >= 0)
      vertices[i].texcoord = vertex_texcoords[vertex_texcoord_indices[i]];
    else
      vertices[i].texcoord = glm::vec2(0.f);

    if (!vertex_normals.empty() && i < vertex_normal_indices.size() && vertex_normal_indices[i] >= 0)
      vertices[i].normal = vertex_normals[vertex_normal_indices[i]];
    else
      vertices[i].normal = glm::vec3(0.f, 0.f, 1.f);

    vertices[i].color = glm::vec3(1.f);
  }

  std::cout << "Nr of vertices: " << vertices.size() << std::endl;

  return vertices;
}