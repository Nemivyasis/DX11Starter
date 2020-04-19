#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>
#include "WICTextureLoader.h"
#include "Material.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "Target.h"
#include "Projectile.h"
#include "CollisionManager.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);


private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();
	std::shared_ptr<Mesh> MakeSquare(float centerX, float centerY, float sideSize);

	std::shared_ptr<Mesh> MakePolygon(int numSides, float centerX, float centerY, float radius);

	void SetGlobalPixelShaderInfo(std::shared_ptr<SimplePixelShader> ps);
	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Mode, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	std::vector<std::shared_ptr<Target>> targets;
	std::vector<std::shared_ptr<Projectile>> projectiles;


	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShaderNormalMap;
	std::shared_ptr<SimpleVertexShader> vertexShaderNormalMap;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cloverTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> targetTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockTextureNMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	DirectionalLight dir1 = {};
	DirectionalLight dir2 = {};
	DirectionalLight dir3 = {};
	PointLight point1 = {};

	//Camera class
	std::unique_ptr<Camera> camera;

	// Projectile mesh
	std::shared_ptr<Mesh> sphereMesh;
	// Projectile material
	std::shared_ptr<Material> cloverMat;

	std::unique_ptr<CollisionManager> collisionManeger;
};

