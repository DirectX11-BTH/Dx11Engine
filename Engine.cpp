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

	this->createWindow();
	createInputHandler();
	createDirectX();
	directXHandler->setupPShader(L"PShader.hlsl");
	directXHandler->setupVShader(L"VShader.hlsl");
	directXHandler->setupInputLayout();
	directXHandler->setupDepthBuffer(WIDTH, HEIGHT); //Sets up depth buffer

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

	//Init camera
	Camera::cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);

	Camera::cameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH( //Creates projection space
		0.35f * 3.14f,					//FovAngleY, height angle of perspective in radians
		(float)WIDTH / (float)HEIGHT,	//AspectRatio, width/height of window
		0.1f,							//NearZ, how close we render
		1000.f							//FarZ how far we render
	);

	directXHandler->setupLightBuffer();
	gBuffHandler.init(WIDTH, HEIGHT);
}

void Engine::engineLoop() //The whole function is not run multiple times a second, it initiates a loop at the bottom
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

	/*float fArray2[] =
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
	directXHandler->createVertexBuffer(debugObject2->meshes.at(0));*/
	//delete debugObject;

	EngineObject* debugObject2 = new EngineObject;
	debugObject2->meshes.push_back(ObjParser::readFromObj("./TestModel/cube.obj"));
	debugObject2->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(0.f, 0.f, 0.0f);
	//debugObject2->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	debugObject2->meshes.at(0).worldMatrix = debugObject2->meshes.at(0).worldMatrix * debugObject2->meshes.at(0).translationMatrix;
	directXHandler->createVertexBuffer(debugObject2->meshes.at(0));
	std::cout << "Cube parsed, nr of vertices in debugObject2 is " << debugObject2->meshes.at(0).vertices.size() << std::endl;

	//Get texture name from debugMesh, gotta convert string to wchar_t*
	debugObject2->meshes.at(0).textureName = "./TestModel/" + debugObject2->meshes.at(0).textureName;
	std::wstring longString = std::wstring(debugObject2->meshes.at(0).textureName.begin(), debugObject2->meshes.at(0).textureName.end());
	const wchar_t* longCharArr = longString.c_str();
	//debugObject2->readTextureFromFile(longCharArr);
	debugObject2->readTextureFromFile(L"./Texture.png");
	
	std::cout << "Executed" << std::endl;
	//CREATEING MESH WITH INDEXES ================================================================================
	/*Mesh ourIndexMesh;

	ourIndexMesh.vertices.push_back(Vertex{ -0.8f, 0.8f, 0.5f,		 1.f, 0.f, 0.f, 1.f,	0.f, 0.f,	0, 0, -1 });
	ourIndexMesh.vertices.push_back(Vertex{ 0.8f, -0.5f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.f, 1.f,	0, 0, -1 });
	ourIndexMesh.vertices.push_back(Vertex{ -0.8f, -0.5f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	0.0f, 1.f,	0, 0, -1 });
	ourIndexMesh.vertices.push_back(Vertex{ 0.8f, 0.8f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	1.f, 0.f,	0, 0, -1 });

	ourIndexMesh.indicies.push_back(0);
	ourIndexMesh.indicies.push_back(1);
	ourIndexMesh.indicies.push_back(2);
	ourIndexMesh.indicies.push_back(0);
	ourIndexMesh.indicies.push_back(3);
	ourIndexMesh.indicies.push_back(1);

	//Second debug object -----------------------------------------------------------------------------------
	Mesh ourIndexMesh2;

	ourIndexMesh2.vertices.push_back(Vertex{ -0.8f, 0.8f, 0.5f,		 1.f, 0.f, 0.f, 1.f,	0.f, 0.f,	0, 0, -1 });
	ourIndexMesh2.vertices.push_back(Vertex{ 0.8f, -0.8f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.f, 1.f,	0, 0, -1 });
	ourIndexMesh2.vertices.push_back(Vertex{ -0.8f, -0.8f, 0.5f,	 0.f, 0.f, 1.f, 1.f,	0.0f, 1.f,	0, 0, -1 });
	ourIndexMesh2.vertices.push_back(Vertex{ 0.8f, 0.8f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	1.f, 0.f,	0, 0, -1 });

	ourIndexMesh2.indicies.push_back(0);
	ourIndexMesh2.indicies.push_back(1);
	ourIndexMesh2.indicies.push_back(2);
	ourIndexMesh2.indicies.push_back(0);
	ourIndexMesh2.indicies.push_back(3);
	ourIndexMesh2.indicies.push_back(1);
	
	EngineObject* debugIndexObject2 = new EngineObject;
	//debugObject2->readMesh(fArray2, 6);
	debugIndexObject2->meshes.push_back(ourIndexMesh);
	debugIndexObject2->readTextureFromFile(L"./texture2.png");
	directXHandler->createVertexBuffer(debugIndexObject2->meshes.at(0));
	directXHandler->createIndexBuffer(debugIndexObject2->meshes.at(0));

	EngineObject* debugIndexObject = new EngineObject;
	debugIndexObject->readTextureFromFile(L"./texture.png");
	debugIndexObject->meshes.push_back(ourIndexMesh2);
	directXHandler->createVertexBuffer(debugIndexObject->meshes.at(0));
	directXHandler->createIndexBuffer(debugIndexObject->meshes.at(0));*/
	//CREATEING MESH WITH END ================================================================================

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
		port.MinDepth = 0.0f; //Closest possible to screen Z depth
		port.MaxDepth = 1.0f; //Furthest possible

		//Clear depth every frame - DEPTH
		DxHandler::contextPtr->ClearDepthStencilView(DxHandler::depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		float background_color[4] = { 0.7f, 0.f, 0.f, 1.f };

		//First pass -------------------------------------------------------------------
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Position].renderTargetView, background_color);

		ID3D11RenderTargetView* arr[3] = {
			gBuffHandler.buffers[GBufferType::Position].renderTargetView,
			gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
			gBuffHandler.buffers[GBufferType::Normal].renderTargetView,
			//DxHandler::renderTargetPtr
		};
		directXHandler->contextPtr->OMSetRenderTargets(3, arr, DxHandler::depthStencil); //DEPTH


		directXHandler->contextPtr->RSSetViewports(1, &port);
		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);
		//directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, DxHandler::depthStencil); //DEPTH

		directXHandler->draw(*debugObject2);
		//First pass end -------------------------------------------------------------------


		//Second pass -------------------------------------------------------------------
		directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, DxHandler::depthStencil);


		//These are to 'read' from the textures
		DxHandler::contextPtr->PSSetShaderResources(0, 1, &gBuffHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
		DxHandler::contextPtr->PSSetShaderResources(1, 1, &gBuffHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
		DxHandler::contextPtr->PSSetShaderResources(2, 1, &gBuffHandler.buffers[GBufferType::Position].shaderResourceView); //Position


		//Need to unbind for the next pass
		DxHandler::contextPtr->PSSetShaderResources(0, 1, NULL); //Color
		DxHandler::contextPtr->PSSetShaderResources(1, 1, NULL); //Normal
		DxHandler::contextPtr->PSSetShaderResources(2, 1, NULL); //Position

		//Second pass end -------------------------------------------------------------------
		directXHandler->swapChainPtr->Present(1, 0);
	}
}
