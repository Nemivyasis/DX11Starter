#pragma once
class Collider
{
public:
	Collider(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	bool IsCollidingWith(Collider other);
private:
	float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;
};

