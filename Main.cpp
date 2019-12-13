#include "Engine.h"
#include "DXHandler.h"

int WinMain(HINSTANCE startInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DxHandler::hInstance = startInstance;

	Engine engine;
	engine.initialSetup();
	engine.engineLoop();

	return 0;
};