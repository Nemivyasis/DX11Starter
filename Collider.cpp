#include "Collider.h"
Collider::Collider(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	min = DirectX::XMFLOAT3(minX, minY, minZ);
	max = DirectX::XMFLOAT3(maxX, maxY, maxZ);
}

DirectX::XMFLOAT3 Collider::GetMax()
{
	return max;
}

DirectX::XMFLOAT3 Collider::GetMin()
{
	return min;
}

