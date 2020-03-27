#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	Material(DirectX::XMFLOAT4 tint, float reflect, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResource,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sState, std::shared_ptr<SimpleVertexShader> vShader,
		std::shared_ptr<SimplePixelShader> pShader);
	Material(DirectX::XMFLOAT4 tint, float reflect, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResource,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sState, std::shared_ptr<SimpleVertexShader> vShader,
		std::shared_ptr<SimplePixelShader> pShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nMap);
	DirectX::XMFLOAT4 GetColorTint() const;
	std::shared_ptr<SimplePixelShader> GetPixelShader() const;
	std::shared_ptr<SimpleVertexShader> GetVertexShader() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResource() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetNormalMap() const;
	bool IsNormalMap() const;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() const;
	float GetReflectivity() const;
	void SetColorTint(DirectX::XMFLOAT4 tint);
	void SetReflectivity(float newReflect);
private:
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	float reflectivity;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
};

