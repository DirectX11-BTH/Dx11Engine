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
	this->inputHandler = InputHandler(primaryWindow);
}

Engine::Engine()
{

}


Engine::~Engine()
{
	delete directXHandler;
}

void Engine::initialSetup()
{
	DxHandler::WIDTH = WIDTH;
	DxHandler::HEIGHT = HEIGHT;
	//ShowCursor(false);
	this->createWindow();
	createDirectX();
	directXHandler->setupPShader(L"PShader.hlsl");
	directXHandler->setupVShader(L"VShader.hlsl");
	directXHandler->setupInputLayout();
	directXHandler->setupDepthBuffer(WIDTH, HEIGHT); //Sets up depth buffer

	createInputHandler();

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

	directXHandler->setupDeferredShaders();

	//Init camera
	Camera::cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);

	Camera::cameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH( //Creates projection space
		0.35f * 3.14f,					//FovAngleY, height angle of perspective in radians
		(float)WIDTH / (float)HEIGHT,	//AspectRatio, width/height of window
		0.1f,							//NearZ, how close we render
		10000.f							//FarZ how far we render
	);

	directXHandler->setupLightBuffer();
	gBuffHandler.init(WIDTH, HEIGHT);
	directXHandler->generateFullscreenQuad();

	D3D11_SAMPLER_DESC textureSamplerDesc;
	textureSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	textureSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.MipLODBias = 0.0f;
	textureSamplerDesc.MaxAnisotropy = 1;
	textureSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	textureSamplerDesc.BorderColor[0] = 0.f;
	textureSamplerDesc.BorderColor[1] = 0.f;
	textureSamplerDesc.BorderColor[2] = 0.f;
	textureSamplerDesc.BorderColor[3] = 0.f;
	textureSamplerDesc.MinLOD = 0;
	textureSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* sampleState;
	HRESULT samplerStateSucc = DxHandler::devicePtr->CreateSamplerState(&textureSamplerDesc, &sampleState);
	assert(SUCCEEDED(samplerStateSucc));

	DxHandler::contextPtr->PSSetSamplers(0, 1, &sampleState);

	//SsaoClass::generateNoiseTexture();
	//SsaoClass::generateRandomVectors();
	//SsaoClass::setupShaders();
	//SsaoClass::generateOcclusionBuffer();
}

void Engine::engineLoop() //The whole function is not run multiple times a second, it initiates a loop at the bottom
{
	terrainGenerator.generateFromHeightMap("./heightmap.png");
	EngineObject terrainObject;
	terrainObject.meshes.push_back(terrainGenerator.heightTerrain);
	terrainObject.readTextureFromFile(L"texture.png");
	//terrainObject.readTextureFromFile(L"texture.png");

	//----------------------------------------------------------------------------------------------- DEBUG

	EngineObject* debugObject = new EngineObject; //cube
	debugObject->meshes.push_back(ObjParser::readFromObj("./TestModel/plane.obj"));
	debugObject->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(0.f, 10.f, -10.0f);
	debugObject->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(10.f, 10.f, 10.f);

	DirectX::XMVECTOR rotAxis = DirectX::XMVectorSet(1, 0, 0, 0);
	debugObject->meshes.at(0).rotationMatrix = DirectX::XMMatrixRotationAxis(rotAxis, 1.78); //radians

	debugObject->meshes.at(0).worldMatrix = debugObject->meshes.at(0).rotationMatrix * debugObject->meshes.at(0).translationMatrix * debugObject->meshes.at(0).scalingMatrix;
	directXHandler->createVertexBuffer(debugObject->meshes.at(0));
	std::cout << "Cube parsed, nr of vertices in debugObject2 is " << debugObject->meshes.at(0).vertices.size() << std::endl;
	//Get texture name from debugMesh, gotta convert string to wchar_t*
	if (debugObject->meshes.at(0).textureName != "")
	{
		debugObject->meshes.at(0).textureName = "./TestModel/" + debugObject->meshes.at(0).textureName;
		std::wstring longString = std::wstring(debugObject->meshes.at(0).textureName.begin(), debugObject->meshes.at(0).textureName.end());
		const wchar_t* longCharArr = longString.c_str();
		debugObject->readTextureFromFile(longCharArr);
	}
	debugObject->hasNormalMap = debugObject->normalMapContainer.loadNormalTextureFromFile(L"./normalMap.png");
	debugObject->readTextureFromFile(L"texture.png");


	/*EngineObject* debugObject2 = new EngineObject;
	debugObject2->meshes.push_back(ObjParser::readFromObj("./TestModel/cube.obj"));
	debugObject2->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(0.f, 10.f, -10.0f);
	debugObject2->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(10.f, 10.f, 10.f);
	debugObject2->meshes.at(0).worldMatrix = debugObject2->meshes.at(0).worldMatrix * debugObject2->meshes.at(0).translationMatrix * debugObject2->meshes.at(0).scalingMatrix;
	directXHandler->createVertexBuffer(debugObject2->meshes.at(0));
	std::cout << "Cube parsed, nr of vertices in debugObject2 is " << debugObject2->meshes.at(0).vertices.size() << std::endl;*/

	//Get texture name from debugMesh, gotta convert string to wchar_t*
	/*if (debugObject2->meshes.at(0).textureName != "")
	{
		debugObject2->meshes.at(0).textureName = "./TestModel/" + debugObject2->meshes.at(0).textureName;
		std::wstring longString = std::wstring(debugObject2->meshes.at(0).textureName.begin(), debugObject2->meshes.at(0).textureName.end());
		const wchar_t* longCharArr = longString.c_str();
		debugObject2->readTextureFromFile(longCharArr);
	}*/
	
	//debugObject2->readTextureFromFile(L"./Texture.png");
	
	std::cout << "Executed" << std::endl;
	//----------------------------------------------------------------------------------------------- END DEBUG

	MSG msg;
	while (true)
	{

		Camera::updateCamera();
		//Update input magic
		inputHandler.handleInput();
		//

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		if (msg.message == WM_QUIT)
		{
			PostQuitMessage(0);
		}

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
		float background_color[4] = { 0.f, 0.f, 0.f, 1.f };
		float background_color2[4] = { 0.9f, 0.6f, 0.6f, 1.f };

		//First pass -------------------------------------------------------------------
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Position].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
		//directXHandler->contextPtr->ClearRenderTargetView(SsaoClass::SSAOBuffRenderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);

		ID3D11RenderTargetView* arr[3] = 
		{
			gBuffHandler.buffers[GBufferType::Position].renderTargetView,
			gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
			gBuffHandler.buffers[GBufferType::Normal].renderTargetView,
		};
		directXHandler->contextPtr->OMSetRenderTargets(3, arr, DxHandler::depthStencil); //DEPTH


		directXHandler->contextPtr->RSSetViewports(1, &port);
		//directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, DxHandler::depthStencil); //DEPTH

		DxHandler::contextPtr->PSSetShader(DxHandler::pixelPtr, NULL, NULL);
		DxHandler::contextPtr->VSSetShader(DxHandler::vertexPtr, NULL, NULL);

		//DxHandler::contextPtr->PSSetShaderResources(1, 1, &debugObject->normalMapContainer.textureView); //NormalMap
		directXHandler->draw(*debugObject);

		//directXHandler->draw(*debugObject2);
		directXHandler->draw(terrainObject);

		//First pass end -------------------------------------------------------------------




		//Second Pass - SSAO Pass Begin ----------------------------------------------------
		/*directXHandler->contextPtr->OMSetRenderTargets(0, NULL, NULL);
		directXHandler->contextPtr->OMSetRenderTargets(1, &SsaoClass::SSAOBuffRenderTargetView, NULL); //Draw to the occlusion buffer

		//Need these to calculate the occlusion factor
		DxHandler::contextPtr->PSSetShaderResources(0, 1, &gBuffHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
		DxHandler::contextPtr->PSSetShaderResources(1, 1, &gBuffHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
		DxHandler::contextPtr->PSSetShaderResources(2, 1, &gBuffHandler.buffers[GBufferType::Position].shaderResourceView); //Position

		DxHandler::contextPtr->PSSetShader(SsaoClass::SSAOPixelPtr, NULL, NULL); //set shaders
		DxHandler::contextPtr->VSSetShader(SsaoClass::SSAOVertexPtr, NULL, NULL);

		DxHandler::contextPtr->PSSetShaderResources(3, 1, &SsaoClass::randomVecShaderResourceView); //Random vectors
		DxHandler::contextPtr->PSSetShaderResources(4, 1, &SsaoClass::randomNoiseShaderResourceView); //Random noise

		directXHandler->drawFullscreenQuad();

		DxHandler::contextPtr->PSSetShaderResources(3, 0, NULL); //Random vectors
		DxHandler::contextPtr->PSSetShaderResources(4, 0, NULL); //Random noise*/

		//----------------------------------------------------------------------------------



		//Third pass -------------------------------------------------------------------
		directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);//, DxHandler::depthStencil);

		//Need these to calculate the occlusion factor
		DxHandler::contextPtr->PSSetShaderResources(0, 1, &gBuffHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
		DxHandler::contextPtr->PSSetShaderResources(1, 1, &gBuffHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
		DxHandler::contextPtr->PSSetShaderResources(2, 1, &gBuffHandler.buffers[GBufferType::Position].shaderResourceView); //Position


		//Do the actual drawing here
		DxHandler::contextPtr->PSSetShader(DxHandler::deferredPixelPtr, NULL, NULL); //set shaders
		DxHandler::contextPtr->VSSetShader(DxHandler::deferredVertexPtr, NULL, NULL);
		
		directXHandler->drawFullscreenQuad(); //Fill in screen with quad to activate all pixels for loading from gbuffs
		
		//Need to unbind for the next pass
		DxHandler::contextPtr->PSSetShaderResources(0, 0, NULL); //Color
		DxHandler::contextPtr->PSSetShaderResources(2, 0, NULL); //Position
		DxHandler::contextPtr->PSSetShaderResources(1, 0, NULL); //Normal

		//Second pass end -------------------------------------------------------------------
		directXHandler->swapChainPtr->Present(1, 0);


	}
}
