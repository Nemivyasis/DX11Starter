#pragma once
#include <DirectXMath.h>
#include "Transform.h"

class Collider
{
public:
	Collider(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	DirectX::XMFLOAT3 GetMax();
	DirectX::XMFLOAT3 GetMin();
private:
	DirectX::XMFLOAT3 max;
	DirectX::XMFLOAT3 min;
};

