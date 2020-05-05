#include "Game.h"
#include "Vertex.h"
#include <iostream>
// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();

	dir1.ambientColor = XMFLOAT3(0.1f, 0.0f, 0.05f);
	dir1.diffuseColor = XMFLOAT3(1, 0, 0.5f);
	dir1.direction = XMFLOAT3(1, 0, 0);

	dir2.ambientColor = XMFLOAT3(0.0f, 0.1f, 0.0f);
	dir2.diffuseColor = XMFLOAT3(0, 1, 0.0f);
	dir2.direction = XMFLOAT3(0, -1, 0);

	dir3.ambientColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
	dir3.diffuseColor = XMFLOAT3(0.9f, 0.9f, 0.9f);
	dir3.direction = XMFLOAT3(-1, 0, 0);

	point1.ambientColor = XMFLOAT3(0.1f, 0.1f, 0.1f);
	point1.diffuseColor = XMFLOAT3(0, 0, 1);
	point1.position = XMFLOAT3(0.1f, 0, 0);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	camera = std::make_unique<Camera>((float)this->width / this->height);

	// for storing projectiles
	// Keep track of projectiles on screen 
	projectiles = std::vector < std::shared_ptr< Projectile >>();

	//Make particle system
	//getTexture
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/particle.jpg").c_str(), 0, particleTexture.GetAddressOf());

	//Create depth state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // turn off
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, particleDepthState.GetAddressOf());

	//blend for particles (addative for now)
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //respect pixelshader alpha
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, particleBlendState.GetAddressOf());

	//Set up actual emitter
	emitter = std::unique_ptr<Emitter>(new Emitter(
		111,
		10,
		6,
		0.1f,
		1.0f,
		XMFLOAT4(0.6f, 0.2f, 0.2f, 1),
		XMFLOAT4(0.3f, 0.3f, 0.3f, 0),
		XMFLOAT3(0, -0.5f, 0),
		XMFLOAT3(0.2f, 0.1f, 0.2f),
		XMFLOAT3(0, 4, 0),
		XMFLOAT3(0.1f, 0.1f, 0.1f),
		XMFLOAT4(-2, 2, -2, 2),
		XMFLOAT3(0, 0.5f, 0),
		device,
		particleVS.get(),
		particlePS.get(),
		particleTexture,
		false,
		true));

	collisionManeger = std::make_unique<CollisionManager>();

	fireRate = 0.5f;
	lastShot = 1.0f;

	ResizePostProcessResources();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"VertexShader.cso").c_str());

	pixelShader = std::make_shared<SimplePixelShader>(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"PixelShader.cso").c_str());

	vertexShaderNormalMap = std::make_shared<SimpleVertexShader>(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"VertexShaderNormal.cso").c_str());

	pixelShaderNormalMap = std::make_shared<SimplePixelShader>(device.Get(), context.Get(),
		GetFullPathTo_Wide(L"PixelShaderNormal.cso").c_str());

	particleVS = std::make_shared<SimpleVertexShader>(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"ParticleVS.cso").c_str());

	particlePS = std::make_shared<SimplePixelShader>(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"ParticlePS.cso").c_str());
		
	//********Post Processing *****************
	ppVS = std::make_shared<SimpleVertexShader>(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"PostProcessVS.cso").c_str());

	ppPS = std::make_shared<SimplePixelShader>(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"PostProcessPS.cso").c_str());

}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	auto cylinderMesh = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device);
	auto cubeMesh = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device);
	auto helixMesh = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device);
	sphereMesh = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device);

	//Make Materials
	/*auto redMaterial = std::make_shared<Material>(XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f), 64, vertexShader, pixelShader);
	auto greenMaterial = std::make_shared<Material>(XMFLOAT4(0.1f, 1, 0.1f, 1.0f), 64, vertexShader, pixelShader);
	auto blueMaterial = std::make_shared<Material>(XMFLOAT4(0.1f, 0.1f, 1, 1.0f), 64, vertexShader, pixelShader);

	auto whiteMaterial = std::make_shared<Material>(XMFLOAT4(1, 1, 1 , 1.0f), 64, vertexShader, pixelShader);*/

	auto hResult = CreateWICTextureFromFile(device.Get(),
		context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/brass.jpg").c_str(),
		nullptr, brassTexture.GetAddressOf());


	hResult = CreateWICTextureFromFile(device.Get(),
		context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/rock.png").c_str(),
		nullptr, rockTexture.GetAddressOf());

	hResult = CreateWICTextureFromFile(device.Get(),
		context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/target.png").c_str(),
		nullptr, targetTexture.GetAddressOf());

	hResult = CreateWICTextureFromFile(device.Get(),
		context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/rock_normals.png").c_str(),
		nullptr, rockTextureNMap.GetAddressOf());


	D3D11_SAMPLER_DESC sampDescription = {};
	sampDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDescription.MaxAnisotropy = 16;
	sampDescription.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDescription, samplerState.GetAddressOf());

	brassMat = std::make_shared<Material>(XMFLOAT4(1, 1, 1.0f, 1.0f), 0, brassTexture, samplerState, vertexShader, pixelShader);
	auto rockMat = std::make_shared<Material>(XMFLOAT4(1, 1, 1.0f, 1.0f), 64.0f, rockTexture, samplerState, vertexShader, pixelShader);
	auto targetMat = std::make_shared<Material>(XMFLOAT4(1, 1, 1.0f, 1.0f), 64.0f, targetTexture, samplerState, vertexShader, pixelShader);
	auto rockMatNMap = std::make_shared<Material>(XMFLOAT4(1, 1, 1.0f, 1.0f), 64.0f, rockTexture, samplerState, vertexShaderNormalMap, pixelShaderNormalMap, rockTextureNMap);

	//Make Targets, each divided into 3 rows.  Rotation is commented out until I get a texture that applies to the top of the cylinders
	targets = std::vector<std::shared_ptr<Target>>();

	//First row of targets
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 10, 5, 5));
	targets.back()->SetPosition(-4, 0, 5);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 10, 5, 5));
	targets.back()->SetPosition(-2, 0, 5);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 10, 5, 5));
	targets.back()->SetPosition(-0, 0, 5);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 10, 5, 5));
	targets.back()->SetPosition(2, 0, 5);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 10, 5, 5));
	targets.back()->SetPosition(4, 0, 5);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);

	//second row of targets
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 20, 7, 5));
	targets.back()->SetPosition(-3, 2, 9);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 20, 7, 5));
	targets.back()->SetPosition(-1, 2, 9);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 20, 7, 5));
	targets.back()->SetPosition(1, 2, 9);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 20, 7, 5));
	targets.back()->SetPosition(3, 2, 9);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);

	//Final row of targets
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 30, 10, 5));
	targets.back()->SetPosition(-2, 4, 13);
	///targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 30, 10, 5));
	targets.back()->SetPosition(-0, 4, 13);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);
	targets.push_back(std::make_shared<Target>(cylinderMesh, targetMat, 30, 10, 5));
	targets.back()->SetPosition(2, 4, 13);
	//targets.back().GetTransform()->Rotate(XM_PIDIV2, 0, 0);

	for (int i = 0; i < targets.size(); i++) {
		//collisionManeger->AddTarget(targets[i]);
	}


}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	if (camera != nullptr) {
		camera->UpdateProjectionMatrix((float)this->width / this->height);
	}

	ResizePostProcessResources();
}


//Creates and resizes the triangle for Post Processing
void Game::ResizePostProcessResources()
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Will render to it and sample from it!
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* ppTexture;
	device->CreateTexture2D(&textureDesc, 0, &ppTexture);

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(ppTexture, &rtvDesc, blurRTV.ReleaseAndGetAddressOf());

	// Create the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	device->CreateShaderResourceView(ppTexture, &srvDesc, ppSRV.ReleaseAndGetAddressOf());

	// We don't need the texture reference itself no mo'
	ppTexture->Release();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	//Draw the entities
	for (size_t i = 0; i < targets.size(); i++)
	{
		targets[i]->Update(deltaTime);//Move the targets
	}

	camera->Update(deltaTime, this->hWnd);

	// -- SHOOTING --

	// Check to see if right mouse button is down
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && lastShot > fireRate)
	{
		lastShot = 0;
		//printf("clicked");
		projectiles.push_back(std::make_shared<Projectile>(
			sphereMesh,
			brassMat,
			10,
			camera.get()->GetTransform()->GetPosition(),
			camera.get()->GetTransform()->GetRotation())
		);
		//collisionManeger->AddBullet(projectiles[projectiles.size() - 1]);
	}

	lastShot += deltaTime;

	// move the projectiles 
	if (projectiles.size() > 0)
	{
		for (size_t i = 0; i < projectiles.size(); i++)
		{
			projectiles[i]->Fire(deltaTime);
		}
	}

	//collisionManeger->Update();

	for (size_t i = 0; i < projectiles.size(); i++)
	{
		// if outside the range, delete the projectile
		if (projectiles[i]->GetPosition().x > 50 || projectiles[i]->GetPosition().x < -50 ||
			projectiles[i]->GetPosition().y > 50 || projectiles[i]->GetPosition().y < -50 ||
			projectiles[i]->GetPosition().z > 50 || projectiles[i]->GetPosition().z < -50)
		{
			projectiles[i]->isDead = true;
		}

		// if collides with target, delete the projectile

		for (int j = 0; j < targets.size(); j++) {
			if (projectiles[i]->IsCollidingWith(*targets[j])) {
				projectiles[i]->isDead = true;
				targets[j]->isDead = true;
			}
		}
	}

	if (!projectiles.empty()) {
		for (int i = projectiles.size() - 1; i >= 0; i--) {
			if (projectiles[i]->isDead) {
				projectiles.erase(projectiles.begin() + i);
			}
		}
	}

	if (!targets.empty()) {
		for (int i = targets.size() - 1; i >= 0; i--) {
			if (targets[i]->isDead) {
				targets.erase(targets.begin() + i);
			}
		}
	}
	
	emitter->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	context->ClearRenderTargetView(blurRTV.Get(), color);

	// Change the render target
	//********Post Processing *****************
	context->OMSetRenderTargets(1, blurRTV.GetAddressOf(), depthStencilView.Get());

	//Set lighting
	SetGlobalPixelShaderInfo(pixelShader);
	SetGlobalPixelShaderInfo(pixelShaderNormalMap);

	//Draw the entities
	for (size_t i = 0; i < targets.size(); i++)
	{
		targets[i]->DrawObject(context.Get(), camera.get());
	}

	// check if there are projectiles
	if (projectiles.size() > 0)
	{
		for (size_t i = 0; i < projectiles.size(); i++)
		{
			projectiles[i]->DrawObject(context.Get(), camera.get());
		}
	}

	//Draw particles
	context->OMSetBlendState(particleBlendState.Get(), 0, 0xffffffff);
	context->OMSetDepthStencilState(particleDepthState.Get(), 0);

	//draw all emitters here
	emitter->Draw(context, camera.get());

	//reset
	context->OMSetBlendState(0, 0, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);

	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), 0);

	//******** Post Processing *****************

	ppVS->SetShader();

	ppPS->SetShaderResourceView("pixels", ppSRV.Get());
	ppPS->SetSamplerState("samplerOptions", samplerState.Get());

	// changes depending on if the camera is moving
	ppPS->SetInt("blurAmount", 3);
	ppPS->SetShader();

	ppPS->SetFloat("pixelWidth", 1.0f / width);
	ppPS->SetFloat("pixelHeight", 1.0f / height);
	ppPS->CopyAllBufferData();

	//// Turn OFF buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* empty = 0;
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &empty, &stride, &offset);

	// Make big triangle
	context->Draw(3, 0);

	//Unbind Shader View
	//******** Post Processing *****************
	ID3D11ShaderResourceView* nullSRVs[16] = {};
	context->PSSetShaderResources(0, 16, nullSRVs);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

//--------------------------------------------
// Makes a square
//--------------------------------------------
std::shared_ptr<Mesh> Game::MakeSquare(float centerX, float centerY, float sideSize)
{
	XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Vertex vertices[] =
	{
		{ XMFLOAT3(centerX + 0.5f * sideSize, centerY + 0.5f * sideSize, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(centerX + 0.5f * sideSize, centerY - 0.5f * sideSize, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(centerX - 0.5f * sideSize, centerY - 0.5f * sideSize, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(centerX - 0.5f * sideSize, centerY + 0.5f * sideSize, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
	};

	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	return std::make_shared<Mesh>(vertices, 4, indices, 6, device);
}

std::shared_ptr<Mesh> Game::MakePolygon(int numSides, float centerX, float centerY, float radius)
{
	if (numSides < 3)
		return NULL;

	XMFLOAT4 red = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Vertex* vertices = new Vertex[(unsigned long long) numSides + 1];
	unsigned int* indices = new unsigned int[(unsigned long long) numSides * 3];
	double angle = (2 * 3.141592f) / numSides;

	//make the first two vertices
	vertices[0] = { XMFLOAT3(centerX, centerY, 0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) };
	vertices[1] = { XMFLOAT3(centerX + radius, centerY, 0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) };

	for (int i = 0; i < (unsigned long long) numSides - 1; i++)
	{
		float xOffset = radius * (float)cos(angle * (i + 1.0));
		float yOffset = radius * (float)sin(angle * (i + 1.0));
		vertices[i + 2] = { XMFLOAT3(centerX + xOffset, centerY + yOffset, 0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) };

		indices[3 * i] = 0;
		indices[3 * i + 1] = i + 2;
		indices[3 * i + 2] = i + 1;
	}

	//make the final side indices
	int i = numSides - 1;
	indices[3 * i] = 0;
	indices[3 * i + 1] = 1;
	indices[3 * i + 2] = i + 1;

	auto mesh = std::make_shared<Mesh>(vertices, numSides + 1, indices, numSides * 3, device);
	delete[] vertices;
	delete[] indices;
	return mesh;
}

void Game::SetGlobalPixelShaderInfo(std::shared_ptr<SimplePixelShader> ps) {
	//Set lighting
	ps->SetData(
		"light",
		&dir1,
		sizeof(DirectionalLight));

	ps->SetData(
		"light2",
		&dir2,
		sizeof(DirectionalLight));

	ps->SetData(
		"light3",
		&dir3,
		sizeof(DirectionalLight));

	ps->SetData(
		"light4",
		&point1,
		sizeof(PointLight));

	ps->SetData(
		"cameraPosition",
		&(camera->GetTransform()->GetPosition()),
		sizeof(XMFLOAT3));

	ps->CopyAllBufferData();
}
