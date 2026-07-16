#pragma once

#include "render/renderable.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Material;
class Shader;
class Texture;
class Mesh;

class Model : public Renderable
{
protected:
	Material *material;
	Texture *overrideTextureDiffuse;
	Texture *overrideTextureSpecular;
	Mesh *mesh;
	glm::mat4 modelMatrix;
	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;

	void updateUniforms(Shader &shader);
	void updateModelMatrix();

public:
	Model(glm::vec3 position, Material &material,
				Mesh &mesh,
				Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
				glm::quat orientation = glm::quat(1.f, 0.f, 0.f, 0.f), glm::vec3 scale = glm::vec3(1.f));

	Model(glm::vec3 position, Mesh &mesh);

	Model(const Model &model);

	// OBJ consturctor
	Model(glm::vec3 position, Material &material,
				const std::string &OBJfile,
				Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr,
				glm::quat orientation = glm::quat(1.f, 0.f, 0.f, 0.f), glm::vec3 scale = glm::vec3(1.f));

	~Model();

	void render(Shader &shader) override;
	void renderInstanced(Shader &shader) override;

	glm::vec3 getPosition() const;
	glm::quat getOrientation() const;

	void setOrientation(const glm::quat &orientation);
	void setScale(const glm::vec3 &scale);
	void setPosition(const glm::vec3 &newPosition);

	const Material *getMaterial() const { return this->material; };
	bool getIsTangent() const;
};