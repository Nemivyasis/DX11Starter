#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter(
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
	SimpleVertexShader* vs, SimplePixelShader* ps, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, 
	bool isSpriteSheet, 
	unsigned int spriteSheetWidth, 
	unsigned int spriteSheetHeight
)
{
	this->maxParticles = maxParticles;
	this->particlesPerSecond = particlesPerSecond;
	this->lifetime = lifetime;
	this->startSize = startSize;
	this->endSize = endSize;
	this->startColor = startColor;
	this->endColor = endColor;
	this->startVelocity = startVelocity;
	this->secondsPerParticle = 1.0f / particlesPerSecond;

	this->positionRandomRange = positionRandomRange;
	this->rotationRandomRanges = rotationRandomRange;
	this->velocityRandomRange = velocityRandomRange;

	this->emitterPosition = emitterPosition;
	this->emitterAcceleration = emitterAcceleration;

	this->vs = vs;
	this->ps = ps;
	this->texture = texture;

	this->isSpriteSheet = isSpriteSheet;
	this->spriteSheetWidth = max(spriteSheetWidth, 1);
	this->spriteSheetHeight = max(spriteSheetHeight, 1);
	this->spriteSheetFrameWidth = 1.0 / this->spriteSheetWidth;
	this->spriteSheetFrameHeight = 1.0 / this->spriteSheetHeight;

	timeSinceEmit = 0;
	livingParticleCount = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;

	particles = new Particle[maxParticles];
	ZeroMemory(particles, sizeof(Particle) * maxParticles);

	DefaultUVs[0] = XMFLOAT2(0, 0);
	DefaultUVs[1] = XMFLOAT2(1, 0);
	DefaultUVs[2] = XMFLOAT2(1, 1);
	DefaultUVs[3] = XMFLOAT2(0, 1);

	localParticleVertices = new ParticleVertex[4 * maxParticles];

	for (int i = 0; i < maxParticles * 4; i += 4) {
		localParticleVertices[i + 0].UV = DefaultUVs[0];
		localParticleVertices[i + 1].UV = DefaultUVs[1];
		localParticleVertices[i + 2].UV = DefaultUVs[2];
		localParticleVertices[i + 3].UV = DefaultUVs[3];
	}

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	device->CreateBuffer(&vbDesc, 0, vertexBuffer.GetAddressOf());

	// Index buffer data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;
	
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, indexBuffer.GetAddressOf());

	delete[] indices;

}

Emitter::~Emitter()
{
	delete[] particles;
	delete[] localParticleVertices;
}

void Emitter::Update(float dt)
{

	// Update all particles - Check cyclic buffer first
	if (firstAliveIndex < firstDeadIndex)
	{
		// First alive is BEFORE first dead, so the "living" particles are contiguous

		// X X X X * * * * * X X X X X 

		for (int i = firstAliveIndex; i < firstDeadIndex; i++)
			UpdateSingleParticle(dt, i);
	}
	else
	{
		// First alive is AFTER first dead, so the "living" particles wrap around

		// * * * * X X X X X * * * * *  

		// Update first half (from firstAlive to max particles)
		for (int i = firstAliveIndex; i < maxParticles; i++)
			UpdateSingleParticle(dt, i);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < firstDeadIndex; i++)
			UpdateSingleParticle(dt, i);
	}

	// Add to the time
	timeSinceEmit += dt;

	// Enough time to emit?
	while (timeSinceEmit > secondsPerParticle)
	{
		SpawnParticle();
		timeSinceEmit -= secondsPerParticle;
	}
}

void Emitter::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera)
{
}

void Emitter::UpdateSingleParticle(float dt, int index)
{
	//is it alive?
	if (particles[index].Age >= lifetime) {
		return;
	}

	//update age
	particles[index].Age += dt;

	//check for death
	if (particles[index].Age >= lifetime) {
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}

	//calculate age percentage for lerp
	float agePercent = particles[index].Age / lifetime;

	//interpolate color
	XMStoreFloat4(
		&particles[index].Color,
		XMVectorLerp(
			XMLoadFloat4(&startColor),
			XMLoadFloat4(&endColor),
			agePercent
		));

	//interpolate rotation
	particles[index].Rotation = particles[index].RotationStart + agePercent * (particles[index].RotationEnd - particles[index].RotationStart);

	//interpolate size
	particles[index].Size = startSize + agePercent * (endSize - startSize);

	//Set position
	XMVECTOR startPos = XMLoadFloat3(&particles[index].StartPosition);
	XMVECTOR startVel = XMLoadFloat3(&particles[index].StartVelocity);
	XMVECTOR accel = XMLoadFloat3(&emitterAcceleration);
	float t = particles[index].Age;

	//using constant acc fucntion
	XMStoreFloat3(
		&particles[index].Position,
		accel * t * t / 2.0f + startVel * t + startPos);

}

void Emitter::SpawnParticle()
{
	//Check whether to spawn
	if (livingParticleCount == maxParticles)
		return;

	//reset the first dead particle
	particles[firstDeadIndex].Age = 0;
	particles[firstDeadIndex].Size = startSize;
	particles[firstDeadIndex].Color = startColor;

	particles[firstDeadIndex].StartPosition = emitterPosition;
	particles[firstDeadIndex].StartPosition.x += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.x;
	particles[firstDeadIndex].StartPosition.y += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.y;
	particles[firstDeadIndex].StartPosition.z += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.z;

	particles[firstDeadIndex].Position = particles[firstDeadIndex].StartPosition;

	particles[firstDeadIndex].StartVelocity = startVelocity;
	particles[firstDeadIndex].StartVelocity.x += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.x;
	particles[firstDeadIndex].StartVelocity.y += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.y;
	particles[firstDeadIndex].StartVelocity.z += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.z;

	float rotStartMin = rotationRandomRanges.x;
	float rotStartMax = rotationRandomRanges.y;
	float rotEndMin = rotationRandomRanges.z;
	float rotEndMax = rotationRandomRanges.w;

	particles[firstDeadIndex].RotationStart = ((float)rand() / RAND_MAX) * (rotStartMax - rotStartMin) + rotStartMin;
	particles[firstDeadIndex].RotationEnd = ((float)rand() / RAND_MAX) * (rotEndMax - rotEndMin) + rotEndMin;

	//increment and warp
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	livingParticleCount++;
}

void Emitter::CopyParticlesToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera)
{
}

void Emitter::CopyOneParticle(int index, Camera* camera)
{
}

DirectX::XMFLOAT3 Emitter::CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, Camera* camera)
{
	return DirectX::XMFLOAT3();
}
