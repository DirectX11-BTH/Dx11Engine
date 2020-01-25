#include "Engine.h"
#include "DXHandler.h"

int WinMain(HINSTANCE startInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DxHandler::hInstance = startInstance;

	Engine engine;
	engine.initialSetup(); //Will set up all the DXStuff, and quite literally everything else
	engine.engineLoop(); //Starts the rendering loop

	return 0;
};