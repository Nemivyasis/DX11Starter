#pragma once
#include<memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat);
	std::shared_ptr<Mesh> GetMesh() const;
	Transform* GetTransform() const;
	std::shared_ptr<Material> GetMaterial() const;

	void DrawObject(ID3D11DeviceContext* context, Camera* camera);
private:
	std::shared_ptr<Mesh> mesh;
	std::unique_ptr<Transform> transform;
	std::shared_ptr<Material> material;
};

