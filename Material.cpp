#include "Material.h"

Material::Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vShader, std::shared_ptr<SimplePixelShader> pShader)
{
	colorTint = tint;
	pixelShader = pShader;
	vertexShader = vShader;
}

DirectX::XMFLOAT4 Material::GetColorTint() const
{
	return colorTint;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader() const
{
	return pixelShader;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() const
{
	return vertexShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 tint)
{
	colorTint = tint;
}
