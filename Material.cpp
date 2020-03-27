#include "Material.h"

Material::Material(DirectX::XMFLOAT4 tint, float reflect, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResource,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sState, std::shared_ptr<SimpleVertexShader> vShader,
	std::shared_ptr<SimplePixelShader> pShader)
{
	colorTint = tint;
	samplerState = sState;
	shaderResourceView = shaderResource;
	reflectivity = reflect;
	pixelShader = pShader;
	vertexShader = vShader;
}

Material::Material(DirectX::XMFLOAT4 tint, float reflect, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResource, Microsoft::WRL::ComPtr<ID3D11SamplerState> sState, std::shared_ptr<SimpleVertexShader> vShader, std::shared_ptr<SimplePixelShader> pShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nMap)
{
	colorTint = tint;
	samplerState = sState;
	shaderResourceView = shaderResource;
	reflectivity = reflect;
	pixelShader = pShader;
	vertexShader = vShader;
	normalMap = nMap;
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

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetShaderResource() const
{
	return shaderResourceView;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetNormalMap() const
{
	return normalMap;
}

bool Material::IsNormalMap() const
{
	return normalMap.Get() != nullptr;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::GetSamplerState() const
{
	return samplerState;
}

float Material::GetReflectivity() const
{
	return reflectivity;
}

void Material::SetColorTint(DirectX::XMFLOAT4 tint)
{
	colorTint = tint;
}

void Material::SetReflectivity(float newReflect)
{
	reflectivity = newReflect;
}
