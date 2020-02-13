#pragma once
#include<memory>
#include "Mesh.h"
#include "Transform.h"
#include "BufferStructs.h"

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> meshptr);
	std::shared_ptr<Mesh> GetMesh() const;
	Transform* GetTransform() const;

	void DrawObject(ID3D11Buffer* constantBufferVS, ID3D11DeviceContext* context);
private:
	std::shared_ptr<Mesh> mesh;
	std::unique_ptr<Transform> transform;
};

