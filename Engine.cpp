#include "Engine.h"
#include "DXHandler.h"

void Engine::createWindow()
{
	this->window = RenderWindow();
	this->primaryWindow = this->window.createWindow(600, 500, "SpaghettiEngine", "Title");
}

void Engine::createDirectX()
{
	this->directXHandler = new DxHandler(primaryWindow);

	//this->directXHandler->configureSwapChain(primaryWindow);
	//this->directXHandler->initalizeDeviceContextAndSwapChain();

}

void Engine::createInputHandler()
{
	this->inputHandler = InputHandler();
}

void Engine::initialSetup()
{
	this->createWindow();
	createInputHandler();
	createDirectX();
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

		RECT viewportRect;
		GetClientRect(primaryWindow, &viewportRect);
		D3D11_VIEWPORT port = {
			0.f,
			0.f,
			(float)(viewportRect.right - viewportRect.left),
			(float)(viewportRect.bottom - viewportRect.top),0.f,1.f
		};
		directXHandler->contextPtr->RSSetViewports(1, &port);
		float background_color[4] = { 0.5f, 0.f, 0.f, 0.f };
		directXHandler->contextPtr->ClearRenderTargetView(directXHandler->renderTargetPtr, background_color);
		directXHandler->contextPtr->OMSetRenderTargets(1, &directXHandler->renderTargetPtr, NULL);

		directXHandler->swapChainPtr->Present(1, 0);
	}
}
