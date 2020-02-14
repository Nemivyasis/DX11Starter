#include "Material.h"

Material::Material(DirectX::XMFLOAT4 tint, Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader)
{
	colorTint = tint;
	pixelShader = pShader;
	vertexShader = vShader;
}

DirectX::XMFLOAT4 Material::GetColorTint() const
{
	return colorTint;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> Material::GetPixelShader() const
{
	return pixelShader;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> Material::GetVertexShader() const
{
	return vertexShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 tint)
{
	colorTint = tint;
}
