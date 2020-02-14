#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>

class Material
{
public:
	Material(DirectX::XMFLOAT4 tint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader,  
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader);
	DirectX::XMFLOAT4 GetColorTint() const;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() const;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() const;

	void SetColorTint(DirectX::XMFLOAT4 tint);
private:
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
};

