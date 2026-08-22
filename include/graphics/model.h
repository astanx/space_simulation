#pragma once

#include "render/renderable.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Material;
class Texture;
class Mesh;

enum class ModelFlags : uint32_t
{
	None = 0,
	CastsShadow = 1 << 0,
	ReflectsLight = 1 << 1,
	Special = 1 << 2,
};

inline ModelFlags operator|(ModelFlags a, ModelFlags b)
{
	return static_cast<ModelFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline ModelFlags operator&(ModelFlags a, ModelFlags b)
{
	return static_cast<ModelFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

class Model : public Renderable
{
protected:
	Material *material;
	Texture *overrideTextureDiffuse;
	Texture *overrideTextureSpecular;
	Mesh *mesh;

	unsigned int impostorLayer;
	float importance;

	ModelFlags flags;

	uint32_t id;

public:
	Model(Material &material, Mesh &mesh, ModelFlags flags = ModelFlags::None, Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr);
	Model(Mesh &mesh, ModelFlags flags = ModelFlags::None);
	Model(const Model &model);
	// OBJ consturctor
	Model(Material &material, const std::string &OBJfile, ModelFlags flags = ModelFlags::None, Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr);

	~Model();

	virtual void render(Shader &shader) const override;
	virtual void renderInstanced(Shader &shader, Buffer *instanceVBO = nullptr, size_t size = 0, size_t count = 0, size_t offset = 0) const override;

	void setImpostorLayer(unsigned int layer) { this->impostorLayer = layer; };
	void setImportance(float importance) { this->importance = importance; };
	void setID(uint32_t id) { this->id = id; };

	const Material *getMaterial() const { return this->material; };
	unsigned int getImpostorLayer() const { return this->impostorLayer; };
	float getImportance() const { return this->importance; };
	const uint32_t getID() const { return this->id; };
	bool getIsTangent() const;
	const glm::vec3 &getAverageColor() const;

	bool hasFlag(ModelFlags flag) const { return (this->flags & flag) == flag; };
	bool hasAnyFlag() const { return this->flags != ModelFlags::None; };
};