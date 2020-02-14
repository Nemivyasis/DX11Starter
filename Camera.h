#pragma once
#include <Windows.h>
#include <memory>
#include "Transform.h"

class Camera
{
public:
	Camera(float aspectRatio);
	Camera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 orientation, float aspectRatio,
		float fieldView, float nearClp, float farClp,
		float moveSpd, float fastMoveSpd, float mouseLookSpd);

	//TODO make more constructors

	DirectX::XMFLOAT4X4 GetViewMatrix() const;
	DirectX::XMFLOAT4X4 GetProjectionMatrix() const;
	Transform* GetTransform() const;

	//TODO make setters
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt, HWND windowHandle);
private:
	std::unique_ptr<Transform> transform;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	POINT prevMousePosition;

	float fov;
	float nearClip;
	float farClip;
	float moveSpeed;
	float fastMoveSpeed;
	float mouseLookSpeed;

	void SetAllCustomOptions(float fieldView, float nearClp, float farClp,
		float moveSpd, float fastMoveSpd, float mouseLookSpd);
	void CreateTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 orientation);
};

