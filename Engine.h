#pragma once
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"
#include "EngineObject.h"
#include "Camera.h"
#include "ObjParser.h"
#include "GBufferHandler.h"

const int WIDTH = 600;
const int HEIGHT = 500;

class Engine
{
private:
	InputHandler inputHandler;
	RenderWindow window;
	DxHandler* directXHandler;
	Camera camera;
	DirectX::XMMATRIX worldViewProjectionMatrix;
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	ID3D11Buffer* PSConstBuff = NULL;
	PS_CONSTANT_LIGHT_BUFFER lightBuff;

	HWND primaryWindow;

	GBufferHandler gBuffHandler;

	void createWindow();
	void createDirectX();
	void createInputHandler();

	void createDebugMesh();
public:
	~Engine();
	Engine();

	void initialSetup();
	void engineLoop();
};




