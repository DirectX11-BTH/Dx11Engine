#pragma once
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"

static ID3D11Device* devicePtr;
static ID3D11DeviceContext* contextPtr;
static HINSTANCE hInstance;

class Engine
{
private:
	InputHandler* inputHandler;
	RenderWindow* window;

	void createWindow();
	void createDirectX();
	void createInputHandler();
public:
	
	void initialSetup();
	void engineLoop();
};




