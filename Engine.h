#pragma once


const int WIDTH = 1080;
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

	void initialSetup();
	void engineLoop();
};




