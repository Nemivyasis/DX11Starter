#pragma once
#include <vector>
#include "Entity.h"

using namespace std;

class CollisionManager
{
public:
	CollisionManager();
	CollisionManager(vector<shared_ptr<Entity>> entities);
	void AddEntity(shared_ptr<Entity> entity);
	void RemoveEntity(int pos);
	void Update();
	void ResolveCollision(Entity &a, Entity &b);

private:
	vector<shared_ptr<Entity>> entities;
};

