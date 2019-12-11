#include "Engine.h"

void Engine::createWindow()
{
	this->window = new RenderWindow();
	this->window->createWindow(600, 500, "SpaghettiEngine", "Title");

}

void Engine::initialSetup()
{
	this->createWindow();
}

void Engine::engineLoop()
{
	MSG msg;
	while (true)
	{

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		if (msg.message == WM_QUIT)
		{
			PostQuitMessage(0);
			break;
		}
		// Run game code here
		// ...
		// ...

		//UPDATING CONSTANT BUFFER -------------------------------------------------
	}
}
