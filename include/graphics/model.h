#pragma once

#include <vector>
#include <glm/glm.hpp>

class Material;
class Shader;
class Texture;
class Mesh;

class Model
{
protected:
	Material *material;
	Texture *overrideTextureDiffuse;
	Texture *overrideTextureSpecular;
	std::vector<Mesh *> meshes;
	glm::mat4 ModelMatrix;
	glm::vec3 position;
	glm::dmat3 orientation;
	glm::vec3 scale;

	void updateUniforms(Shader &shader);
	void updateModelMatrix();

public:
	Model(glm::vec3 position, Material &material,
				std::vector<Mesh *> meshes,
				Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
				glm::mat3 orientation = glm::mat3(1.f), glm::vec3 scale = glm::vec3(1.f));

	Model(glm::vec3 position, Material &material,
				Mesh &mesh,
				Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
				glm::mat3 orientation = glm::mat3(1.f), glm::vec3 scale = glm::vec3(1.f));

	Model(const Model &model);

	// OBJ consturctor
	Model(glm::vec3 position, Material &material,
				const std::string &OBJfile,
				Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
				glm::mat3 orientation = glm::mat3(1.f), glm::vec3 scale = glm::vec3(1.f));

	~Model();

	void render(Shader &shader);
	void renderInstanced();

	glm::vec3 getPosition() const;
	glm::mat3 getOrientation() const;

	void setOrientation(const glm::mat3 &orientation);
	void scaleBy(const glm::vec3 &scale);
	void setPosition(const glm::vec3 &newPosition);
};