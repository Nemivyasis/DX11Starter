#include "Transform.h"
using namespace DirectX;

Transform::Transform()
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	worldDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3(pitch, yaw, roll);
	worldDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	worldDirty = true;
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetRotation()
{
	return rotation;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (worldDirty)
		RecalculateWorldMatrix();
	return worldMatrix;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR move = XMVectorSet(x, y, z, 0);
	pos += move;
	XMStoreFloat3(&position, pos);
	worldDirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR move = XMVectorSet(x, y, z, 0);
	XMVECTOR rot = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	move = XMVector3Rotate(move, rot);

	XMVECTOR pos = XMLoadFloat3(&position);
	pos += move;

	XMStoreFloat3(&position, pos);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	XMVECTOR rot = XMLoadFloat3(&rotation);
	XMVECTOR change = XMVectorSet(pitch, yaw, roll, 0);
	rot += change;
	XMStoreFloat3(&rotation, rot);
	worldDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	XMVECTOR scaleVec = XMLoadFloat3(&scale);
	XMVECTOR change = XMVectorSet(x, y, z, 0);
	scaleVec *= change;
	XMStoreFloat3(&scale, scaleVec);
	worldDirty = true;
}

void Transform::RecalculateWorldMatrix()
{
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);

	XMMATRIX world = scaleMat * rotationMat * translationMat;

	XMStoreFloat4x4(&worldMatrix, world);

	worldDirty = false;
}
