#pragma once
#include <vector>
#include "Entity.h"

using namespace std;

class CollisionManager
{
public:
	CollisionManager();
	CollisionManager(vector<Entity> entities);
	void AddEntity(Entity &entity);
	void RemoveEntity(int pos);
	void Update();
	void ResolveCollision(Entity &a, Entity &b);

private:
	vector<Entity> entities;
};

