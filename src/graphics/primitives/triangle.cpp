#include "graphics/primitives/triangle.h"

#include "graphics/vertex.h"

// Constructor and Destructor

Triangle::Triangle() : Primitive()
{
	std::vector<Vertex> vertices =
		 {
			  {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f)},
			  {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)},
			  {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f)}};

	std::vector<GLuint> indices =
		 {
			  0,
			  1,
			  2,
		 };

	this->set(vertices, indices);
}