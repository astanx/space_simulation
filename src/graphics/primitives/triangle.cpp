#include "graphics/primitives/triangle.h"

#include "graphics/vertex.h"

// Constructor and Destructor

Triangle::Triangle() : Primitive()
{
	this->positions.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
	this->positions.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
	this->positions.push_back(glm::vec3(0.5f, 0.5f, 0.0f));

	this->texcoords.push_back(glm::vec2(0.f, 0.f));
	this->texcoords.push_back(glm::vec2(1.f, 0.f));
	this->texcoords.push_back(glm::vec2(1.f, 1.f));

	this->normals.push_back(glm::vec3(0.f, 0.f, 1.f));
	this->normals.push_back(glm::vec3(0.f, 0.f, 1.f));
	this->normals.push_back(glm::vec3(0.f, 0.f, 1.f));

	this->indices = {0, 1, 2};
}