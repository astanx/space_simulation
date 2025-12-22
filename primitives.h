#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "vertex.h"

class Primitive
{
private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

public:
  Primitive() {}
  virtual ~Primitive() {}

  void set(const Vertex *vertices, const unsigned nrOfVertices, const GLuint *indices, const unsigned nrOfIndices)
  {
    for (size_t i = 0; i < nrOfVertices; i++)
    {
      this->vertices.push_back(vertices[i]);
    }

    for (size_t i = 0; i < nrOfIndices; i++)
    {
      this->indices.push_back(indices[i]);
    }
  }

  inline Vertex *getVertices()
  {
    return this->vertices.data();
  }
  inline GLuint *getIndices()
  {
    return this->indices.data();
  }
  inline const unsigned getNrOfVertices()
  {
    return this->vertices.size();
  }
  inline const unsigned getNrOfIndices()
  {
    return this->indices.size();
  }
};

class Triangle : public Primitive
{
public:
  Triangle() : Primitive()
  {
    Vertex vertices[] =
        {
            glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f),
            glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f),
            glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f),
            glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 1.f),
        };

    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3
        };

    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
  }
};


class Quad : public Primitive
{
public:
  Quad() : Primitive()
  {
    Vertex vertices[] =
        {
            glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f),
            glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f),
            glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f),
            glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 1.f),
        };

    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3
        };

    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
  }
};

class Cube : public Primitive
{
public:
  Cube() : Primitive()
  {
    Vertex vertices[] =
		{
				// Front face (+Z)
				{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f)},
				{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)},
				{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f)},
				{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 1.f)},

				// Back face (-Z)
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, -1.f)},

				// Left face (-X)
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)},

				// Right face (+X)
				{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(1.f, 0.f, 0.f)},

				// Top face (+Y)
				{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},

				// Bottom face (-Y)
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)},
    };

    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] =
        {
            0, 1, 2, 0, 2, 3,				// Front
            4, 5, 6, 4, 6, 7,				// Back
            8, 9, 10, 8, 10, 11,		// Left
            12, 13, 14, 12, 14, 15, // Right
            16, 17, 18, 16, 18, 19, // Top
            20, 21, 22, 20, 22, 23	// Bottom
        };

    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
  }
};