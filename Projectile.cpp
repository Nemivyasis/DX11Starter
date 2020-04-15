#include "Projectile.h"

Projectile::Projectile(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat, float speed, DirectX::XMFLOAT3 camPos) : Entity(meshptr, mat)
{
	this->speed = speed;
	this->camPos = camPos;
	SetPosition();
}

void Projectile::Fire()
{
	GetTransform()->MoveRelative(0, 0, speed);
}

void Projectile::SetPosition()
{
	GetTransform()->SetPosition(camPos.x, camPos.y, camPos.z);
}
