#pragma once


const int WIDTH = 720;
const int HEIGHT = 720;

#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"
#include "EngineObject.h"
#include "Camera.h"
#include "ObjParser.h"
#include "GBufferHandler.h"
#include "TerrainGenerator.h"
#include "SsaoClass.h"
#include "EnvironmentCube.h"
#include "WaterPlane.h"

class Engine
{
private:
	InputHandler inputHandler;
	RenderWindow window;
	DxHandler* directXHandler;
	Camera camera;
	DirectX::XMMATRIX worldViewProjectionMatrix;
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	TerrainGenerator terrainGenerator;
	EngineObject terrain;

	WaterPlane waterGenerator;
	EngineObject waterObject;

	ID3D11Buffer* PSConstBuff = NULL;
	PS_CONSTANT_LIGHT_BUFFER lightBuff;

	HWND primaryWindow;

	GBufferHandler gBuffHandler;

	void createWindow();
	void createDirectX();
	void createInputHandler();

	EnvironmentCube reflectingCube;
public:
	~Engine();
	Engine();

	void drawEnvironmentCube(D3D11_VIEWPORT &originalViewport);
	void initialSetup();
	void engineLoop();
};




