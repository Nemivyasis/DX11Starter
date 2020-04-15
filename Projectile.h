#pragma once
#include "Entity.h"

class Projectile: public Entity
{
	public:
		Projectile(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat, float speed, DirectX::XMFLOAT3 camPos);
		void Fire();
		void SetPosition();
	private:
		float speed;
		DirectX::XMFLOAT3 camPos;
		void Update();
		void DestroyProjectile();
};

