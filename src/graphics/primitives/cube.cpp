#include "graphics/primitives/cube.h"

#include "graphics/vertex.h"

// Constructor and Destructor
Cube::Cube() : Primitive()
{
  // Front face (+Z)
  this->positions.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
  this->positions.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
  this->positions.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
  this->positions.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));

  this->texcoords.push_back(glm::vec2(0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 1.0f));
  this->texcoords.push_back(glm::vec2(0.0f, 1.0f));

  this->normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
  this->normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
  this->normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
  this->normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

  // Back face (-Z)
  this->positions.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
  this->positions.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
  this->positions.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
  this->positions.push_back(glm::vec3(0.5f, 0.5f, -0.5f));

  this->texcoords.push_back(glm::vec2(0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 1.0f));
  this->texcoords.push_back(glm::vec2(0.0f, 1.0f));

  this->normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
  this->normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
  this->normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
  this->normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));

  // Left face (-X)
  this->positions.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
  this->positions.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
  this->positions.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
  this->positions.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));

  this->texcoords.push_back(glm::vec2(0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 1.0f));
  this->texcoords.push_back(glm::vec2(0.0f, 1.0f));

  this->normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
  this->normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
  this->normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
  this->normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));

  // Right face (+X)
  this->positions.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
  this->positions.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
  this->positions.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
  this->positions.push_back(glm::vec3(0.5f, 0.5f, 0.5f));

  this->texcoords.push_back(glm::vec2(0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 1.0f));
  this->texcoords.push_back(glm::vec2(0.0f, 1.0f));

  this->normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
  this->normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
  this->normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
  this->normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

  // Top face (+Y)
  this->positions.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
  this->positions.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
  this->positions.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
  this->positions.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));

  this->texcoords.push_back(glm::vec2(0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 1.0f));
  this->texcoords.push_back(glm::vec2(0.0f, 1.0f));

  this->normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
  this->normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
  this->normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
  this->normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

  // Bottom face (-Y)
  this->positions.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
  this->positions.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
  this->positions.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
  this->positions.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));

  this->texcoords.push_back(glm::vec2(0.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 0.0f));
  this->texcoords.push_back(glm::vec2(1.0f, 1.0f));
  this->texcoords.push_back(glm::vec2(0.0f, 1.0f));

  this->normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
  this->normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
  this->normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
  this->normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

  this->indices =
      {
          0, 1, 2, 0, 2, 3,       // Front
          4, 5, 6, 4, 6, 7,       // Back
          8, 9, 10, 8, 10, 11,    // Left
          12, 13, 14, 12, 14, 15, // Right
          16, 17, 18, 16, 18, 19, // Top
          20, 21, 22, 20, 22, 23  // Bottom
      };
}