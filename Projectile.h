#pragma once
#include "Entity.h"

class Projectile: public Entity
{
	public:
		Projectile(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat, float speed, DirectX::XMFLOAT3 camPos, DirectX::XMFLOAT3 camRot);
		void Fire(float deltaTime);
		void SetPosition();
		DirectX::XMFLOAT3 GetPosition();
		void SetRotation();
		DirectX::XMFLOAT3 GetRotation();
		bool isDead;
	private:
		float speed;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 camPos;
		DirectX::XMFLOAT3 camRot;
};

