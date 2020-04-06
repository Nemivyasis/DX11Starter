#include "Entity.h"
#include <iostream>
Entity::Entity(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat)
{
	mesh = meshptr;
	transform = std::make_unique<Transform>();
	material = mat;
}

std::shared_ptr<Mesh> Entity::GetMesh() const
{
	return mesh;
}

Transform* Entity::GetTransform() const
{
	return transform.get();
}

std::shared_ptr<Material> Entity::GetMaterial() const
{
	return material;
}

bool Entity::IsCollidingWith(Entity& other)
{
	Collider* thisColl = mesh->GetCollider();
	Collider* otherColl = mesh->GetCollider();

	XMFLOAT3 maxColl;
	XMStoreFloat3(&maxColl, XMVector3Transform(XMLoadFloat3(&thisColl->GetMax()), XMLoadFloat4x4(&transform->GetRotlessWorldMatrix())));
	XMFLOAT3 minColl;
	XMStoreFloat3(&minColl, XMVector3Transform(XMLoadFloat3(&thisColl->GetMin()), XMLoadFloat4x4(&transform->GetRotlessWorldMatrix())));

	XMFLOAT3 otherMaxColl;
	XMStoreFloat3(&otherMaxColl, XMVector3Transform(XMLoadFloat3(&otherColl->GetMax()), XMLoadFloat4x4(&other.transform->GetWorldMatrix())));
	XMFLOAT3 otherMinColl;
	XMStoreFloat3(&otherMinColl, XMVector3Transform(XMLoadFloat3(&otherColl->GetMin()), XMLoadFloat4x4(&other.transform->GetWorldMatrix())));

	if (!(maxColl.x >= otherMinColl.x && otherMaxColl.x >= minColl.x))
		return false;

	if (!(maxColl.y >= otherMinColl.y && otherMaxColl.y >= minColl.y))
		return false;

	if (!(maxColl.z >= otherMinColl.z && otherMaxColl.z >= minColl.z))
		return false;

	return true;
}

void Entity::DrawObject(ID3D11DeviceContext* context, Camera* camera)
{
	auto ps = material->GetPixelShader();
	auto vs = material->GetVertexShader();
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vs->SetShader();
	ps->SetShader();



	//Set the constant buffer
	vs->SetFloat4("colorTint", material->GetColorTint());
	vs->SetMatrix4x4("worldMatrix", transform->GetWorldMatrix());
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vs->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix());
	
	vs->CopyAllBufferData();

	ps->SetFloat("reflectivity", material->GetReflectivity());
	ps->SetSamplerState("samplerOptions", material->GetSamplerState().Get());
	ps->SetShaderResourceView("diffuseTexture", material->GetShaderResource().Get());
	
	if (material->IsNormalMap()) {
		ps->SetShaderResourceView("normalMap", material->GetNormalMap().Get());
	}

	ps->CopyAllBufferData();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	//Draw them
	context->DrawIndexed(
		mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}
