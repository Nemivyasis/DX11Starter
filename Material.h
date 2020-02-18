#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "SimpleShader.h"
#include <memory>

class Material
{
public:
	Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vShader, 
		std::shared_ptr<SimplePixelShader> pShader);
	DirectX::XMFLOAT4 GetColorTint() const;
	std::shared_ptr<SimplePixelShader> GetPixelShader() const;
	std::shared_ptr<SimpleVertexShader> GetVertexShader() const;

	void SetColorTint(DirectX::XMFLOAT4 tint);
private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
};

