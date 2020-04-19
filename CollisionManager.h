#pragma once
#include <vector>
#include "Entity.h"
#include "Target.h"

using namespace std;

class CollisionManager
{
public:
	CollisionManager();
	void AddTarget(shared_ptr<Entity> entity);
	void AddBullet(shared_ptr<Entity> entity);
	void SetTargets(vector<shared_ptr<Entity>> targets);
	void RemoveTarget(int pos);
	void RemoveBullet(int pos);
	void Update();
	void ResolveCollision(Entity &a, Entity &b);


private:
	vector<shared_ptr<Entity>> targets;
	vector<shared_ptr<Entity>> bullets;
};

