#include "Projectile.h"

Projectile::Projectile(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat, float speed, DirectX::XMFLOAT3 camPos, DirectX::XMFLOAT3 camRot) : Entity(meshptr, mat)
{
	this->speed = speed;
	this->camPos = camPos;
	this->camRot = camRot;
	this->isDead = false;
	SetPosition();
	SetRotation();
}

void Projectile::Fire(float deltaTime)
{
	GetTransform()->MoveRelative(0, 0, speed*deltaTime);
}

void Projectile::SetPosition()
{
	GetTransform()->SetPosition(camPos.x, camPos.y, camPos.z+3);
}

DirectX::XMFLOAT3 Projectile::GetPosition()
{
	return GetTransform()->GetPosition();
}

void Projectile::SetRotation()
{
	GetTransform()->SetRotation(camRot.x, camRot.y, camRot.z);
}

DirectX::XMFLOAT3 Projectile::GetRotation()
{
	return GetTransform()->GetRotation();
}
