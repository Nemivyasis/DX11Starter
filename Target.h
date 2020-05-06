#pragma once
#include "Entity.h"
class Target :
	public Entity
{//Extention of Entity, that holds the amount of points you get for shooting a target, and how quick the target will move
	public:
		Target(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat, float pointValue, float speed, float range);
		float GetPoints();
		float GetSpeed();
		void Update(float deltaTime);
		void SetPosition(float x, float y, float z);

		bool isDead;
	private:
		float pointValue;
		float speed;
		float range;//How far right/left of their origin the targets will move
		XMFLOAT3 origin;
		float delay;
		float lastSwitch;

		float Distance(XMFLOAT3 origin, XMFLOAT3 current);
};

