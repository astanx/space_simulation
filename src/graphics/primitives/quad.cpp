#include "graphics/primitives/quad.h"
#include "graphics/vertex.h"

// Constructor and Destructor
Quad::Quad() : Primitive()
{
    Vertex vertices[] =
        {
            {glm::vec3(-0.5f, -0.5f, 0.0f),
             glm::vec3(1.f, 0.f, 0.f),
             glm::vec2(0.f, 0.f),
             glm::vec3(0.f, 0.f, 1.f)},
            
             {glm::vec3(0.5f, -0.5f, 0.0f),
             glm::vec3(0.f, 1.f, 0.f),
             glm::vec2(1.f, 0.f),
             glm::vec3(0.f, 0.f, 1.f)},
            
             {glm::vec3(0.5f, 0.5f, 0.0f),
             glm::vec3(0.f, 0.f, 1.f),
             glm::vec2(1.f, 1.f),
             glm::vec3(0.f, 0.f, 1.f)},
            
             {glm::vec3(-0.5f, 0.5f, 0.0f),
             glm::vec3(1.f, 1.f, 0.f),
             glm::vec2(0.f, 1.f),
             glm::vec3(0.f, 0.f, 1.f)},
        };

    unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

    GLuint indices[] =
        {
            0, 1, 2,
            0, 2, 3};

    unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

    this->set(vertices, nrOfVertices, indices, nrOfIndices);
}