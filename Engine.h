#pragma once
#include "DXHandler.h"
#include "RenderWindow.h"
#include "InputHandler.h"

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
public:
	
	void initialSetup();
	void engineLoop();
};




