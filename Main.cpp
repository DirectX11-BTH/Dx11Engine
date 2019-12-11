#include "Engine.h"

int WinMain(HINSTANCE startInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInstance = startInstance;

	Engine engine;
	engine.initialSetup();
	engine.engineLoop();

	return 0;
};