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

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	camera = std::make_unique<Camera>((float)this->width / this->height);
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
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	//Make Materials
	auto redMaterial = std::make_shared<Material>(XMFLOAT4(1.0f, 0, 0, 1.0f), vertexShader, pixelShader);
	auto greenMaterial = std::make_shared<Material>(XMFLOAT4(0, 1, 0, 1.0f), vertexShader, pixelShader);
	auto blueMaterial = std::make_shared<Material>(XMFLOAT4(0, 0, 1, 1.0f), vertexShader, pixelShader);

	//Make Entities
	entities = std::vector<Entity>();
	entities.push_back(Entity(std::make_shared<Mesh>(vertices, 3, indices, 3, device), redMaterial));
	auto squareMesh = MakeSquare(0, 0, 0.25f);
	entities.push_back(Entity(squareMesh, greenMaterial));
	entities.back().GetTransform()->SetPosition(0.75f, 0.75f, 0);
	entities.push_back(Entity(squareMesh, redMaterial));
	entities.back().GetTransform()->SetPosition(-0.75f, 0, 0);
	entities.push_back(Entity(squareMesh, blueMaterial));
	entities.back().GetTransform()->SetPosition(0.75f, -.75f, 0);
	entities.push_back(Entity(MakePolygon(360, -0.75f, -0.75f, 0.2f), greenMaterial));
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
		camera->UpdateProjectionMatrix((float) this->width / this->height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	entities[0].GetTransform()->Rotate(0, 0, XM_PIDIV4 * deltaTime);
	entities[1].GetTransform()->Rotate(0, 0, -XM_PIDIV4 * deltaTime);
	entities[2].GetTransform()->Scale(1 - (0.1f * deltaTime), 1, 1);
	entities[3].GetTransform()->MoveAbsolute(-.1f * deltaTime, 0, 0);
	entities.back().GetTransform()->MoveAbsolute(0, .1f * deltaTime, 0);

	camera->Update(deltaTime, this->hWnd);
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

	//Draw the entities
	for (size_t i = 0; i < entities.size(); i++)
	{
		entities[i].DrawObject(context.Get(), camera.get());
	}

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
		float xOffset = radius * (float) cos(angle * (i + 1.0));
		float yOffset = radius * (float) sin(angle * (i + 1.0));
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
