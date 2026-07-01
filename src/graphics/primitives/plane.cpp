#include "graphics/primitives/plane.h"

#include "graphics/vertex.h"

// Constructor and Destructor
Plane::Plane() : Primitive()
{
	this->positions.push_back(glm::vec3(-0.5f, 0.f, 0.5f));
	this->positions.push_back(glm::vec3(0.5f, 0.f, 0.5f));
	this->positions.push_back(glm::vec3(0.5f, 0.f, -0.5f));
	this->positions.push_back(glm::vec3(-0.5f, 0.f, -0.5f));

	this->texcoords.push_back(glm::vec2(0.f, 1.f));
	this->texcoords.push_back(glm::vec2(1.f, 1.f));
	this->texcoords.push_back(glm::vec2(1.f, 0.f));
	this->texcoords.push_back(glm::vec2(0.f, 0.f));

	this->normals.push_back(glm::vec3(0.f, 1.f, 0.f));
	this->normals.push_back(glm::vec3(0.f, 1.f, 0.f));
	this->normals.push_back(glm::vec3(0.f, 1.f, 0.f));
	this->normals.push_back(glm::vec3(0.f, 1.f, 0.f));

	this->indices =
			{0, 1, 2,
			 0, 2, 3};
}