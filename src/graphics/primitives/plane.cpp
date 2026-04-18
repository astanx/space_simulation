#include "graphics/primitives/plane.h"

#include "graphics/vertex.h"

// Constructor and Destructor
Plane::Plane() : Primitive()
{
	std::vector<Vertex> vertices =
			{
					{glm::vec3(-0.5f, 0.f, 0.5f), glm::vec3(1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},
					{glm::vec3(0.5f, 0.f, 0.5f), glm::vec3(1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},
					{glm::vec3(0.5f, 0.f, -0.5f), glm::vec3(1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
					{glm::vec3(-0.5f, 0.f, -0.5f), glm::vec3(1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
			};

	unsigned nrOfVertices = 4;

	std::vector<GLuint> indices =
			{
					0, 1, 2,
					0, 2, 3};

	this->set(vertices, indices);
}