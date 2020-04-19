#include "CollisionManager.h"

CollisionManager::CollisionManager()
{
	this->targets = std::vector<std::shared_ptr<Entity>>();
	this->bullets = std::vector<std::shared_ptr<Entity>>();
}

void CollisionManager::AddTarget(shared_ptr<Entity> entity)
{
	this->targets.push_back(entity);
	printf("1");
}

void CollisionManager::AddBullet(shared_ptr<Entity> entity)
{
	bullets.push_back(entity);
	printf("1");
}

void CollisionManager::RemoveTarget(int pos)
{
	if (pos < 0) {
		return;
	}

	if (pos > targets.size() - 1) {
		return;
	}

	vector<shared_ptr<Entity>>::iterator it = targets.begin();

	it += pos - 1;

	targets.erase(it);
}

void CollisionManager::RemoveBullet(int pos)
{
	if (pos < 0) {
		return;
	}

	if (pos > bullets.size() - 1) {
		return;
	}

	vector<shared_ptr<Entity>>::iterator it = bullets.begin();

	it += pos - 1;

	bullets.erase(it);
}


void CollisionManager::Update()
{
	
	if (bullets.size() != 0 && targets.size() != 0) {
		for (int i = 0; i < bullets.size(); i++) {
			for (int j = 0; j < targets.size(); j++) {
				if (bullets[i]->IsCollidingWith(*targets[j])) {
					ResolveCollision(*bullets[i], *targets[j]);
				}
			}
		}
	}
}

void CollisionManager::ResolveCollision(Entity& a, Entity& b)
{
	a.GetMaterial()->SetColorTint(DirectX::XMFLOAT4(1, 0, 0, 1));
	b.GetMaterial()->SetColorTint(DirectX::XMFLOAT4(1, 0, 0, 1));
}
