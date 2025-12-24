#include "cube.h"
#include "render/vertex/vertex.h"

// Constructor and Destructor
Cube::Cube() : Primitive()
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
            0, 1, 2, 0, 2, 3,       // Front
            4, 5, 6, 4, 6, 7,       // Back
            8, 9, 10, 8, 10, 11,    // Left
            12, 13, 14, 12, 14, 15, // Right
            16, 17, 18, 16, 18, 19, // Top
            20, 21, 22, 20, 22, 23  // Bottom
        };

    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
}