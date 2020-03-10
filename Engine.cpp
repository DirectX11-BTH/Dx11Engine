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

	//Set up intitial shaders
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

	directXHandler->setupDeferredShaders();
	directXHandler->setupGShader(L"GeometryShader.hlsl");
	directXHandler->createGSConstBuffer();

	//Init camera stuff
	Camera::cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);
	Camera::cameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH( //Creates projection space
		0.35f * 3.14f,					//FovAngleY, height angle of perspective in radians
		(float)WIDTH / (float)HEIGHT,	//AspectRatio, width/height of window
		0.1f,							//NearZ, how close we render
		10000.f							//FarZ how far we render
	);

	//Pixel shader const buffer
	directXHandler->setupLightBuffer();

	//Create gbuffers for deferred
	gBuffHandler.init(WIDTH, HEIGHT);

	//Full screen quad for deferred
	directXHandler->generateFullscreenQuad();

	//Constant sampler for all textures. Could be refactored later.
	D3D11_SAMPLER_DESC textureSamplerDesc;
	textureSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	textureSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	textureSamplerDesc.MipLODBias = 0.0f;
	textureSamplerDesc.MaxAnisotropy = 1;
	textureSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDesc.BorderColor[0] = 0.5f;
	textureSamplerDesc.BorderColor[1] = 0.f;
	textureSamplerDesc.BorderColor[2] = 0.f;
	textureSamplerDesc.BorderColor[3] = 0.f;
	textureSamplerDesc.MinLOD = 0;
	textureSamplerDesc.MaxLOD = 0;

	ID3D11SamplerState* sampleState;
	HRESULT samplerStateSucc = DxHandler::devicePtr->CreateSamplerState(&textureSamplerDesc, &sampleState);
	assert(SUCCEEDED(samplerStateSucc));

	DxHandler::contextPtr->PSSetSamplers(0, 1, &sampleState);
	DxHandler::setupComputeShader();

	//Generate static gaussian kernel for blurring & intermediate output textures.
	DxHandler::gaussianTexture = DxHandler::generateGaussianKernel();
	DxHandler::generateGaussianTextures();
}

void Engine::engineLoop() //The whole function is not run multiple times a second, it initiates a loop at the bottom
{
	
	EngineObject* debugObject2 = new EngineObject; //Texture cube, no glow.
	debugObject2->meshes.push_back(ObjParser::readFromObj("./TestModel/actualCube.obj"));
	debugObject2->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(25, 25, 25);
	debugObject2->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(25, 200, 150);
	debugObject2->meshes.at(0).worldMatrix = debugObject2->meshes.at(0).scalingMatrix * debugObject2->meshes.at(0).rotationMatrix * debugObject2->meshes.at(0).translationMatrix;
	if (debugObject2->meshes.at(0).textureName != "")
	{
		debugObject2->meshes.at(0).textureName = "./TestModel/" + debugObject2->meshes.at(0).textureName;
		std::wstring longString = std::wstring(debugObject2->meshes.at(0).textureName.begin(), debugObject2->meshes.at(0).textureName.end());
		const wchar_t* longCharArr = longString.c_str();
		debugObject2->readTextureFromFile(longCharArr);
	}
	directXHandler->createVertexBuffer(debugObject2->meshes.at(0));
	debugObject2->readTextureFromFile(L"./TestModel/texture.png");


	EngineObject* debugObject4 = new EngineObject; ///Glowing cube
	debugObject4->glowingObject = true;
	debugObject4->meshes.push_back(ObjParser::readFromObj("./TestModel/actualCube.obj"));
	debugObject4->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(25, 25, 25);
	debugObject4->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(25, 200, 25);
	debugObject4->meshes.at(0).worldMatrix = debugObject4->meshes.at(0).scalingMatrix * debugObject4->meshes.at(0).rotationMatrix * debugObject4->meshes.at(0).translationMatrix;
	if (debugObject4->meshes.at(0).textureName != "")
	{
		debugObject4->meshes.at(0).textureName = "./TestModel/" + debugObject4->meshes.at(0).textureName;
		std::wstring longString = std::wstring(debugObject4->meshes.at(0).textureName.begin(), debugObject4->meshes.at(0).textureName.end());
		const wchar_t* longCharArr = longString.c_str();
		debugObject4->readTextureFromFile(longCharArr);
	}
	directXHandler->createVertexBuffer(debugObject4->meshes.at(0));


	terrainGenerator.generateFromHeightMap("./heightmap.png"); //Read in heightmap and generate terrain.
	EngineObject terrainObject;
	terrainObject.meshes.push_back(terrainGenerator.heightTerrain);
	terrainObject.meshes.at(0).worldMatrix = DirectX::XMMatrixTranslation(0, 100, 0); //Position terrain

	reflectingCube.buildCameras(1000.f, 200.f, 1000.f); //Set up environment mapping cameras for cube based on position.
	reflectingCube.object.meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(1000.f, 200.f, 1000.f);
	reflectingCube.object.meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(45.f, 45.f, 45.0f);
	reflectingCube.object.meshes.at(0).worldMatrix = reflectingCube.object.meshes.at(0).scalingMatrix * reflectingCube.object.meshes.at(0).translationMatrix;
	reflectingCube.buildCubeMap(); //Generate cube mesh and place out in the world.
	DxHandler::createVertexBuffer(reflectingCube.object.meshes.at(0));

	EngineObject* debugObject = new EngineObject; //normal mapped plane
	debugObject->meshes.push_back(ObjParser::readFromObj("./TestModel/plane.obj"));
	debugObject->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(-10.f, 250.f, -50.0f);
	debugObject->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(20.f, 10.f, 20.f);
	debugObject->meshes.at(0).worldMatrix = debugObject->meshes.at(0).scalingMatrix * debugObject->meshes.at(0).rotationMatrix * debugObject->meshes.at(0).translationMatrix;
	directXHandler->createVertexBuffer(debugObject->meshes.at(0));
	DirectX::XMVECTOR rotAxis = DirectX::XMVectorSet(1, 0, 0, 0);
	if (debugObject->meshes.at(0).textureName != "") //Loads in parsed texture.
	{
		debugObject->meshes.at(0).textureName = "./TestModel/" + debugObject->meshes.at(0).textureName;
		std::wstring longString = std::wstring(debugObject->meshes.at(0).textureName.begin(), debugObject->meshes.at(0).textureName.end());
		const wchar_t* longCharArr = longString.c_str();
		debugObject->readTextureFromFile(longCharArr);
	}
	debugObject->hasNormalMap = debugObject->normalMapContainer.loadNormalTextureFromFile(L"./normalMap2.jpg"); //Loads in normal map, returns bool if succeeded.


	waterGenerator.generateWaterPlane(); //Water
	waterObject.meshes.push_back(waterGenerator.waterGeometry); 
	waterObject.readTextureFromFile(L"waterTexture.png");
	waterObject.meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(40.f, 1.f, 40.0f);
	waterObject.meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(-900.f, 120.f, -900.0f);
	waterObject.meshes.at(0).worldMatrix = waterObject.meshes.at(0).scalingMatrix * waterObject.meshes.at(0).translationMatrix;

	EngineObject* debugObject3 = new EngineObject; //Table object for texture demonstration.
	debugObject3->meshes.push_back(ObjParser::readFromObj("./TestModel/Wood_Table.obj"));
	debugObject3->meshes.at(0).translationMatrix = DirectX::XMMatrixTranslation(100.f, 200.f, 150.0f);
	debugObject3->meshes.at(0).scalingMatrix = DirectX::XMMatrixScaling(70.f, 70.f, 70.f);
	debugObject3->meshes.at(0).worldMatrix = debugObject3->meshes.at(0).scalingMatrix * debugObject3->meshes.at(0).worldMatrix * debugObject3->meshes.at(0).translationMatrix;
	directXHandler->createVertexBuffer(debugObject3->meshes.at(0));
	std::cout << "Cube parsed, nr of vertices in debugObject3 is " << debugObject3->meshes.at(0).vertices.size() << std::endl;
	if (debugObject3->meshes.at(0).textureName != "") //Load in parsed texture from TestModel folder.
	{
		debugObject3->meshes.at(0).textureName = "./TestModel/" + debugObject3->meshes.at(0).textureName;
		std::wstring longString = std::wstring(debugObject3->meshes.at(0).textureName.begin(), debugObject3->meshes.at(0).textureName.end());
		const wchar_t* longCharArr = longString.c_str();
		debugObject3->readTextureFromFile(longCharArr);
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
	float background_color[4] = { 0.f, 0.f, 0.f, 1.f };

	MSG msg;
	ID3D11ShaderResourceView* nullSRV = { NULL }; //Null SRV
	
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


		if (msg.message == WM_DESTROY)
		{
			PostQuitMessage(0);
		}

		//Clear depth every frame - DEPTH
		DxHandler::contextPtr->ClearDepthStencilView(DxHandler::depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//===================================================================================
		//Cube map stuff --------------------------------------------------------------------
		directXHandler->contextPtr->RSSetViewports(1, &reflectingCube.port);
		for (int i = 0; i < 6; i++)
		{

			//First pass (reflection cube) ---------------------------------------------------------------------------------------------------------------------
			DxHandler::contextPtr->PSSetShader(DxHandler::pixelPtr, NULL, NULL);
			DxHandler::contextPtr->VSSetShader(DxHandler::vertexPtr, NULL, NULL);

			//Clears RTVs
			directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Position].renderTargetView, background_color);
			directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
			directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
			directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Glow].renderTargetView, background_color);
			directXHandler->contextPtr->ClearRenderTargetView(reflectingCube.renderTargetView[i], background_color);

			DxHandler::contextPtr->ClearDepthStencilView(reflectingCube.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			//Set RTVs one gbuffer for each attribute, merges in our fullsrceenquad pass
			ID3D11RenderTargetView* arr[4] =
			{
				gBuffHandler.buffers[GBufferType::Position].renderTargetView,
				gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
				gBuffHandler.buffers[GBufferType::Normal].renderTargetView,
				gBuffHandler.buffers[GBufferType::Glow].renderTargetView
			};
			//
			directXHandler->contextPtr->OMSetRenderTargets(4, arr, reflectingCube.depthStencilView); //DEPTH

			//Draw everything in the scene but the reflection cube itself. Not allowed to reflect itself. We fill our buffers here with draw data.
			directXHandler->draw(reflectingCube.faceCameras[i], waterObject, true); //Draw the water
			directXHandler->draw(reflectingCube.faceCameras[i], *debugObject);
			directXHandler->draw(reflectingCube.faceCameras[i], *debugObject2);
			directXHandler->draw(reflectingCube.faceCameras[i], *debugObject3);
			directXHandler->draw(reflectingCube.faceCameras[i], *debugObject4);
			directXHandler->draw(reflectingCube.faceCameras[i], terrainObject); //Draw the terrain

			//Second pass (reflection cube) -------------------------------------------------------------------------------------------------------------
			//Tell DX11 to output to our graphics (application) window.
			directXHandler->contextPtr->OMSetRenderTargets(1, &reflectingCube.renderTargetView[i], reflectingCube.depthStencilView); //Binds to other output.
			//Above the gbuffers are unbound as output so that they can be bound as input to the lightpixelshader here.
			
			//Make gbuffers available to 2nd [DeferredLightPixel] pass shader for read operation.
			DxHandler::contextPtr->PSSetShaderResources(0, 1, &gBuffHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
			DxHandler::contextPtr->PSSetShaderResources(1, 1, &gBuffHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
			DxHandler::contextPtr->PSSetShaderResources(2, 1, &gBuffHandler.buffers[GBufferType::Position].shaderResourceView); //Position
			DxHandler::contextPtr->PSSetShaderResources(3, 1, &gBuffHandler.buffers[GBufferType::Glow].shaderResourceView); //Glow

			//Set to 2nd pass shaders - reads from gbuffs and outputs to main window.
			DxHandler::contextPtr->PSSetShader(DxHandler::deferredPixelPtr, NULL, NULL); //Tell DX11 to use this pixel shader.
			DxHandler::contextPtr->VSSetShader(DxHandler::deferredVertexPtr, NULL, NULL); //Tell DX11 to use this vertex shader.

			//Actually draws
			directXHandler->drawFullscreenQuad(); //Fill in screen with quad to activate all pixels for loading from gbuffs

			//Need to unbind for the next pass
			DxHandler::contextPtr->PSSetShaderResources(0, 1, &nullSRV); //Color
			DxHandler::contextPtr->PSSetShaderResources(1, 1, &nullSRV); //Normal			
			DxHandler::contextPtr->PSSetShaderResources(2, 1, &nullSRV); //Position
			DxHandler::contextPtr->PSSetShaderResources(3, 1, &nullSRV); //Glow
		}

		//Ensure we're not using reflection cube's port.
		directXHandler->contextPtr->RSSetViewports(1, &port);
		//End cube stuff --------------------------------------------------------------------
		//===================================================================================



		//First pass -------------------------------------------------------------------
		DxHandler::contextPtr->GSSetShader(DxHandler::geometryPtr, NULL, NULL);
		
		//Clear RTVs again
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Position].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Normal].renderTargetView, background_color);
		directXHandler->contextPtr->ClearRenderTargetView(gBuffHandler.buffers[GBufferType::Glow].renderTargetView, background_color);

		directXHandler->contextPtr->ClearRenderTargetView(DxHandler::renderTargetPtr, background_color);

		ID3D11RenderTargetView* arr[4] = 
		{
			gBuffHandler.buffers[GBufferType::Position].renderTargetView,
			gBuffHandler.buffers[GBufferType::DiffuseColor].renderTargetView,
			gBuffHandler.buffers[GBufferType::Normal].renderTargetView,
			gBuffHandler.buffers[GBufferType::Glow].renderTargetView,
		};
		directXHandler->contextPtr->OMSetRenderTargets(4, arr, DxHandler::depthStencil); //DEPTH
		directXHandler->contextPtr->RSSetViewports(1, &port);

		//Set first pass shaders again
		DxHandler::contextPtr->PSSetShader(DxHandler::pixelPtr, NULL, NULL);
		DxHandler::contextPtr->VSSetShader(DxHandler::vertexPtr, NULL, NULL);
		DxHandler::contextPtr->PSSetShaderResources(2, 1, &reflectingCube.shaderResourceView); //Slot 3 for reflectingCubeTexture

		//Draws scene
		directXHandler->draw(*debugObject);
		directXHandler->draw(waterObject, false, true);
		directXHandler->draw(terrainObject);
		directXHandler->draw(*debugObject2);
		directXHandler->draw(*debugObject3);
		directXHandler->draw(*debugObject4);

		//Draw the reflecting cube
		directXHandler->draw(reflectingCube.object, true);

		//Set GSShader to NULL.
		DxHandler::contextPtr->GSSetShader(NULL, NULL, NULL);

		//Set render target to screen output.
		directXHandler->contextPtr->OMSetRenderTargets(1, &DxHandler::renderTargetPtr, NULL);
		//First pass end -------------------------------------------------------------------


		//Blur & glow pass - blurs the glow gbuffer.
		DxHandler::blurTexture(gBuffHandler.buffers[GBufferType::Glow].renderTargetTexture, gBuffHandler.buffers[GBufferType::Glow].shaderResourceView);
		//


		//Second pass -------------------------------------------------------------------
		
		//Make gbuffer SRVs available to 2nd pass shader.
		DxHandler::contextPtr->PSSetShaderResources(0, 1, &gBuffHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Color
		DxHandler::contextPtr->PSSetShaderResources(1, 1, &gBuffHandler.buffers[GBufferType::Normal].shaderResourceView); //Normal
		DxHandler::contextPtr->PSSetShaderResources(2, 1, &gBuffHandler.buffers[GBufferType::Position].shaderResourceView); //Position
		DxHandler::contextPtr->PSSetShaderResources(3, 1, &gBuffHandler.buffers[GBufferType::Glow].shaderResourceView); //Glow

		//Set shaders
		DxHandler::contextPtr->PSSetShader(DxHandler::deferredPixelPtr, NULL, NULL); //set shaders
		DxHandler::contextPtr->VSSetShader(DxHandler::deferredVertexPtr, NULL, NULL);

		directXHandler->drawFullscreenQuad(); //Fill in screen with quad to activate all pixels for loading from gbuffs

		//Need to unbind for the next pass
		DxHandler::contextPtr->PSSetShaderResources(0, 1, &nullSRV); //Color
		DxHandler::contextPtr->PSSetShaderResources(1, 1, &nullSRV); //Normal
		DxHandler::contextPtr->PSSetShaderResources(2, 1, &nullSRV); //Position
		DxHandler::contextPtr->PSSetShaderResources(3, 1, &nullSRV); //Glow


		//Second pass end -------------------------------------------------------------------
		directXHandler->swapChainPtr->Present(1, 0);


	}
}
