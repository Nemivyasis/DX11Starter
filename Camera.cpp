#include "Camera.h"
using namespace DirectX;

Camera::Camera(float aspectRatio)
{
	SetAllCustomOptions(XM_PIDIV4, .01f, 500, 1, 3, .005f);
	CreateTransform(XMFLOAT3(0, 0, -10), XMFLOAT3(0, 0, 0));
	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
}

Camera::Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 orientation, float aspectRatio,
	float fieldView, float nearClip, float farClip, 
	float moveSpd, float fastMoveSpd, float mouseLookSpd)
{
	SetAllCustomOptions(fieldView, nearClip, farClip, moveSpd, fastMoveSpd, mouseLookSpd);
	CreateTransform(pos, orientation);
	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

Transform* Camera::GetTransform() const
{
	return transform.get();
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	auto projMat = XMMatrixPerspectiveFovLH(fov, aspectRatio,
		nearClip, farClip);
	XMStoreFloat4x4(&projectionMatrix, projMat);
}

void Camera::UpdateViewMatrix()
{
	auto pos = XMLoadFloat3(&transform->GetPosition());
	auto rot = XMQuaternionRotationRollPitchYaw(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z);
	auto viewMat = XMMatrixLookToLH(pos, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rot), XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&viewMatrix, viewMat);
}

void Camera::Update(float dt, HWND windowHandle)
{
	//choose the move speed
	float currentMoveSpeed = moveSpeed;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		currentMoveSpeed = fastMoveSpeed;

	//Update move
	if (GetAsyncKeyState('W') & 0x8000)
		transform->MoveRelative(0, 0, currentMoveSpeed * dt);
	if (GetAsyncKeyState('S') & 0x8000)
		transform->MoveRelative(0, 0, -currentMoveSpeed * dt);
	if (GetAsyncKeyState('A') & 0x8000)
		transform->MoveRelative(-currentMoveSpeed * dt, 0, 0);
	if (GetAsyncKeyState('D') & 0x8000)
		transform->MoveRelative(currentMoveSpeed * dt, 0, 0);
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		transform->MoveAbsolute(0, currentMoveSpeed * dt, 0);
	if (GetAsyncKeyState('X') & 0x8000)
		transform->MoveAbsolute(0, -currentMoveSpeed * dt, 0);

	//mouse stuff
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		POINT moveDist = {};
		moveDist.x = mousePos.x - prevMousePosition.x;
		moveDist.y = mousePos.y - prevMousePosition.y;
		
		float xRot = (moveDist.x * mouseLookSpeed);
		float yRot = (moveDist.y * mouseLookSpeed);
		transform->Rotate(yRot, xRot, 0);
	}

	prevMousePosition = mousePos;

	UpdateViewMatrix();
}

void Camera::SetAllCustomOptions(float fieldView, float nearClp, float farClp, float moveSpd, float fastMoveSpd, float mouseLookSpd)
{
	fov = fieldView;
	nearClip = nearClp;
	farClip = farClp;
	moveSpeed = moveSpd;
	fastMoveSpeed = fastMoveSpd;
	mouseLookSpeed = mouseLookSpd;
}

void Camera::CreateTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 orientation)
{
	transform = std::make_unique<Transform>();
	transform->SetPosition(pos.x, pos.y, pos.z);
	transform->SetRotation(orientation.x, orientation.y, orientation.z);
}
