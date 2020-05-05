#include "Target.h"

Target::Target(std::shared_ptr<Mesh> meshptr, std::shared_ptr<Material> mat, float pointValue, float speed, float range)
    : Entity(meshptr, mat)
{
    this->pointValue = pointValue;
    this->speed = speed;
    this->range = range;
    this->origin = GetTransform()->GetPosition();//Gets the origin of the target
    this->isDead = false;
}

//Gets the point value of the target
float Target::GetPoints() {
    return pointValue;
}

//Gets the speed of the target
float Target::GetSpeed() {
    return speed;
}

void Target::SetPosition(float x, float y, float z) {
    GetTransform()->SetPosition(x, y, z);
    origin = GetTransform()->GetPosition();
}

void Target::Update(float deltaTime) {
    float distance = Distance(origin, GetTransform()->GetPosition());

    //if the target has hit the end of the "track", start moving in the opposite direction
    if (distance+0.5f > range)
        speed = -speed;
    GetTransform()->MoveAbsolute(speed*deltaTime, 0, 0);
}

//helper function to calculate distance between origin and current position
float Target::Distance(XMFLOAT3 origin, XMFLOAT3 current)
{
    XMVECTOR v1 = XMLoadFloat3(&origin);
    XMVECTOR v2 = XMLoadFloat3(&current);
    XMVECTOR vDiff = XMVectorSubtract(v1, v2);
    XMVECTOR length = XMVector3Length(vDiff);

    float distance = 0.0f;
    XMStoreFloat(&distance, length);
    return distance;
}