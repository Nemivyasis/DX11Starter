#include "Collider.h"

Collider::Collider(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	this->minX = minX;
	this->minY = minY;
	this->minZ = minZ;
	this->maxX = maxX;
	this->maxY = maxY;
	this->maxZ = maxZ;
}

bool Collider::IsCollidingWith(Collider &other)
{
	if (!(maxX >= other.minX && other.maxX >= minX))
		return false;

	if (!(maxY >= other.minY && other.maxY >= minY))
		return false;

	if (!(maxZ >= other.minZ && other.maxZ >= minZ))
		return false;

	return true;
}
