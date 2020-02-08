#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> meshptr)
{
	mesh = meshptr;
	transform = std::make_unique<Transform>();
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

Transform* Entity::GetTransform()
{
	return transform.get();
}

void Entity::DrawObject(ID3D11Buffer* constantBufferVS, ID3D11DeviceContext* context)
{
	//Set the constant buffer
	VertexShaderExternalData vsData;
	vsData.colorTint = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vsData.worldMatrix = transform->GetWorldMatrix();

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
