#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <fstream>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include "Vertex.h"
#include "Collider.h"

using namespace DirectX;

class Mesh
{
public:
	Mesh(Vertex* vertices, int vertexCount, 
		unsigned int* indices, int indexCount,
		Microsoft::WRL::ComPtr<ID3D11Device> device);
	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D11Device> device);


	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	Collider* GetCollider();

	int GetIndexCount();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	std::unique_ptr<Collider> collider;

	int indCount;

	void CreateBuffers(Vertex* vertices, int vertexCount,
		unsigned int* indices, int indexCount,
		Microsoft::WRL::ComPtr<ID3D11Device> device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	void CalculateCollider(Vertex* verts, int numVerts);
};

