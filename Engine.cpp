#include "Engine.h"
#include "DXHandler.h"

void Engine::createWindow()
{
	this->window = RenderWindow();
	this->primaryWindow = this->window.createWindow(WIDTH, HEIGHT, "SpaghettiEngine", "Title");
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

Engine::~Engine()
{
	delete directXHandler;
}

void Engine::initialSetup()
{

	//Debug
	EngineObject e;
	ObjParser::readFromObj(".\Cube.obj", e);
	// --------------------------------

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

	//Initiate constant buffers
	VS_CONSTANT_MATRIX_BUFFER* matrixBuff = new VS_CONSTANT_MATRIX_BUFFER;
	//matrixBuff->worldMatrix = DirectX::XMMATRIX();
	
	ID3D11Buffer* VSConstBuff = NULL;
	VSConstBuff = directXHandler->createVSConstBuffer(*matrixBuff);
	DxHandler::contextPtr->UpdateSubresource(VSConstBuff, 0, NULL, &matrixBuff, 0, 0);

	//Camera setup & constant buffer that might not be used?
	//Has to be created after perobject constant buffer so that index in vector (loadedVSBuffers) is equal to the constant CAMERA_CBUFFER_SLOT
	VS_CONSTANT_CAMERA_BUFFER cameraMatrixBuff;
	ID3D11Buffer* VSCameraConstBuff = NULL;
	VSCameraConstBuff = directXHandler->createVSConstBuffer(cameraMatrixBuff);

	Camera::cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);

	Camera::cameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH( //Creates projection space
		0.35f * 3.14f,//FovAngleY, height angle of perspective in radians
		(float)WIDTH / (float)HEIGHT,//AspectRatio, width/height of window
		1.0f,//NearZ, how close we render
		1000.f//FarZ how far we render
	);

	worldViewProjectionMatrix = worldMatrix * camera.cameraView * Camera::cameraProjectionMatrix;
	DxHandler::contextPtr->UpdateSubresource(VSConstBuff, 0, NULL, &Camera::cameraProjectionMatrix, 0, 0);
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
	debugObject->readTextureFromFile(L"./texture.png");
	debugObject->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(
		0.f,    // Units translated on the x-axis
		0.f,    // Units translated on the y-axis
		0.f    // Units translated on the z-axis
	);
	debugObject->meshes.at(0).worldMatrix = debugObject->meshes.at(0).translationMatrix;

	directXHandler->createVertexBuffer(debugObject->meshes.at(0));

	float fArray2[] =
	{
		//		XYZ		//		//       RGBA     //	//  UV  //	//nX nY nZ//
		-0.8f, 0.8f, 0.5f,		 1.f, 0.f, 0.f, 1.f,	0.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.8f, -0.5f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.f, 1.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		-0.8f, -0.5f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	0.0f, 1.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ

		-0.8f, 0.8f, 0.5f,		 1.f, 0.f, 0.f, 0.f,	0.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.8f, 0.8f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	1.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.8f, -0.5f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.0f, 1.f,	0, 0, -1//XYZ RGBA UV nXnYnZ
	};
	EngineObject* debugObject2 = new EngineObject;
	debugObject2->readMesh(fArray2, 6);
	debugObject2->readTextureFromFile(L"./texture2.png");
	directXHandler->createVertexBuffer(debugObject2->meshes.at(0));
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
		float background_color[4] = { 0.7f, 0.f, 0.f, 0.5f };
		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
		directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);

		directXHandler->draw(*debugObject);
		directXHandler->draw(*debugObject2);

		directXHandler->swapChainPtr->Present(1, 0);
	}
}
