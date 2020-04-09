#include "CollisionManager.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::CollisionManager(vector<Entity> entities)
{
	this->entities = entities;
}

void CollisionManager::AddEntity(Entity &entity)
{
	entities.push_back(entity);
}

void CollisionManager::RemoveEntity(int pos)
{
	if (pos < 0) {
		return;
	}

	if (pos > entities.size() - 1) {
		return;
	}

	vector<Entity>::iterator it = entities.begin();

	it += pos - 1;

	entities.erase(it);
}

void CollisionManager::Update()
{
	for (int i = 0; i < entities.size() - 1; i++) {
		int j = i + 1;
		for (; j < entities.size(); j++) {
			if (entities[i].IsCollidingWith(entities[j])) {
				ResolveCollision(entities[i], entities[j]);
			}
		}
	}
}

void CollisionManager::ResolveCollision(Entity& a, Entity& b)
{
}
