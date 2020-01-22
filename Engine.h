#pragma once
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"
#include "EngineObject.h"
#include "Camera.h"
#include "ObjParser.h"

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

	HWND primaryWindow;

	void createWindow();
	void createDirectX();
	void createInputHandler();

	void createDebugMesh();
public:
	~Engine();

	void initialSetup();
	void engineLoop();
};




