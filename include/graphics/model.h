#pragma once

#include "render/renderable.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Material;
class Texture;
class Mesh;

class Model : public Renderable
{
protected:
	Material *material;
	Texture *overrideTextureDiffuse;
	Texture *overrideTextureSpecular;
	Mesh *mesh;

	unsigned int impostorLayer;

public:
	Model(Material &material, Mesh &mesh, Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr);

	Model(Mesh &mesh);

	Model(const Model &model);

	// OBJ consturctor
	Model(Material &material, const std::string &OBJfile, Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr);

	~Model();

	void render(Shader &shader) override;
	void renderInstanced(Shader &shader, Buffer *instanceVBO = nullptr, size_t size = 0, size_t count = 0, size_t offset = 0) override;

	void setImpostorLayer(unsigned int layer) { this->impostorLayer = layer; };

	const Material *getMaterial() const { return this->material; };
	unsigned int getImpostorLayer() { return this->impostorLayer; };
	bool getIsTangent() const;
};