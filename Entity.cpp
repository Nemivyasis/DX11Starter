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

void Entity::DrawObject(ID3D11Buffer* constantBufferVS, ID3D11DeviceContext* context, Camera* camera)
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	context->VSSetShader(material->GetVertexShader().Get(), 0, 0);
	context->PSSetShader(material->GetPixelShader().Get(), 0, 0);

	//Set the constant buffer
	VertexShaderExternalData vsData;
	vsData.colorTint = material->GetColorTint();
	vsData.worldMatrix = transform->GetWorldMatrix();
	vsData.projectionMatrix = camera->GetProjectionMatrix();
	vsData.viewMatrix = camera->GetViewMatrix();

	//Copy the buffer over
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(constantBufferVS, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(constantBufferVS, 0);

	context->VSSetConstantBuffers(
		0,  // Which slot (register) to bind the buffer to?
		1,  // How many are we activating?  Can do multiple at once 
		&constantBufferVS);  // Array of buffers (or the address of one)

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
