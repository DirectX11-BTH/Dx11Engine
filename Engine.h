#pragma once
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"
#include "EngineObject.h"

class Engine
{
private:
	InputHandler inputHandler;
	RenderWindow window;
	DxHandler* directXHandler;

	HWND primaryWindow;

	void createWindow();
	void createDirectX();
	void createInputHandler();

	void createDebugMesh();
public:
	
	void initialSetup();
	void engineLoop();
};




