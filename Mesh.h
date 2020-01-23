#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Vertex.h"

class Mesh
{
public:
	Mesh(Vertex* vertices, int vertexCount, 
		unsigned int* indices, int indexCount,
		Microsoft::WRL::ComPtr<ID3D11Device> device);

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	int GetIndexCount();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indCount;
};

