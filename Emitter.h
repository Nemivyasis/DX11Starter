#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "Camera.h"
#include "SimpleShader.h"

struct Particle
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 StartPosition;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 StartVelocity;
	float Size;
	float Age;
	float RotationStart;
	float RotationEnd;
	float Rotation;
};

struct ParticleVertex {
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Color;
};

class Emitter
{
public:

	Emitter(
		int maxParticles,
		int particlesPerSecond,
		float lifetime,
		float startSize,
		float endSize,
		DirectX::XMFLOAT4 startColor,
		DirectX::XMFLOAT4 endColor,
		DirectX::XMFLOAT3 startVelocity,
		DirectX::XMFLOAT3 velocityRandomRange,
		DirectX::XMFLOAT3 emitterPosition,
		DirectX::XMFLOAT3 positionRandomRange,
		DirectX::XMFLOAT4 rotationRandomRange,
		DirectX::XMFLOAT3 emitterAcceleration,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		SimpleVertexShader* vs,
		SimplePixelShader* ps,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
		bool isOneShot = false,
		bool isActive = true,
		bool isSpriteSheet = false,
		unsigned int spriteSheetWidth = 1,
		unsigned int spriteSheetHeight = 1
	);

	~Emitter();

	void SetEmitterPosition(DirectX::XMFLOAT3 newPos);
	void Update(float dt);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera);

	bool IsActive();
	void SetActive(bool newState);

	void Reset(); //Reset the dead counter so the emitter can loop again

	void SetPosition(float x, float y, float z);
	void SetStartVelocity(float x, float y, float z);
	void SetAcceleration(float x, float y, float z);
private:

	// Emission properties
	int particlesPerSecond;
	float secondsPerParticle;
	float timeSinceEmit;

	bool isActive;
	bool isOneShot;

	bool isSpriteSheet;
	int spriteSheetWidth;
	int spriteSheetHeight;
	float spriteSheetFrameWidth;
	float spriteSheetFrameHeight;

	int livingParticleCount;
	float lifetime;

	DirectX::XMFLOAT2 DefaultUVs[4];

	DirectX::XMFLOAT3 emitterAcceleration;
	DirectX::XMFLOAT3 emitterPosition;
	DirectX::XMFLOAT3 startVelocity;

	DirectX::XMFLOAT3 positionRandomRange;
	DirectX::XMFLOAT3 velocityRandomRange;
	DirectX::XMFLOAT4 rotationRandomRanges; // Min start, max start, min end, max end

	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	float startSize;
	float endSize;

	// Particle array
	Particle* particles;
	int maxParticles;
	int firstDeadIndex;
	int firstAliveIndex;

	// Rendering
	ParticleVertex* localParticleVertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	SimpleVertexShader* vs;
	SimplePixelShader* ps;

	// Update Methods
	void UpdateSingleParticle(float dt, int index);
	void SpawnParticle();

	// Copy methods
	void CopyParticlesToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera);
	void CopyOneParticle(int index, Camera* camera);
	DirectX::XMFLOAT3 CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, Camera* camera);


};

