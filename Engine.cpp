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
	directXHandler->setupPShader(L"PShader.hlsl");
	directXHandler->setupVShader(L"VShader.hlsl");
	directXHandler->setupInputLayout();

	DxHandler::contextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DxHandler::contextPtr->IASetInputLayout((ID3D11InputLayout*)DxHandler::input_layout_ptr);
	DxHandler::contextPtr->VSSetShader((ID3D11VertexShader*)DxHandler::vertexPtr, NULL, 0);
	DxHandler::contextPtr->PSSetShader((ID3D11PixelShader*)DxHandler::pixelPtr, NULL, 0);

	//this->directXHandler->createVertexBuffer()
}

void Engine::engineLoop()
{
	//----------------------------------------------------------------------------------------------- DEBUG
	float fArray[] =
	{
		//		XYZ		//		//       RGBA     //	//  UV  //	//nX nY nZ//
		-0.7f, 0.7f, 0.5f,		 1.f, 0.f, 0.f, 1.f,	0.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.7f, -0.7f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.f, 1.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		-0.7f, -0.7f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	0.0f, 1.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ

		-0.7f, 0.7f, 0.5f,		 1.f, 0.f, 0.f, 0.f,	0.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.7f, 0.7f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	1.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.7f, -0.7f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.0f, 1.f,	0, 0, -1//XYZ RGBA UV nXnYnZ
	};
	EngineObject* debugObject = new EngineObject;
	debugObject->readMesh(fArray, 6);
	directXHandler->createVertexBuffer(debugObject->meshes.at(0));
	//delete debugObject;
	//----------------------------------------------------------------------------------------------- END DEBUG

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
		float background_color[4] = { 1.f, 1.f, 1.f, 0.5f };
		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
		directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);

		directXHandler->draw(*debugObject);

		directXHandler->swapChainPtr->Present(1, 0);
	}
}
