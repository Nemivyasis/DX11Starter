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

void Entity::DrawObject(ID3D11DeviceContext* context, Camera* camera)
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	//Set the constant buffer
	auto vs = material->GetVertexShader();
	vs->SetFloat4("colorTint", material->GetColorTint());
	vs->SetMatrix4x4("worldMatrix", transform->GetWorldMatrix());
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vs->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix());

	vs->CopyAllBufferData();

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
