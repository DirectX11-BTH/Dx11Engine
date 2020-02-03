#include "DXHandler.h"

ID3D11Device* DxHandler::devicePtr = nullptr;
ID3D11DeviceContext* DxHandler::contextPtr = nullptr;
HINSTANCE DxHandler::hInstance = HINSTANCE();

IDXGISwapChain* DxHandler::swapChainPtr = nullptr;
ID3D11RenderTargetView* DxHandler::renderTargetPtr = nullptr;
DXGI_SWAP_CHAIN_DESC DxHandler::swapDesc = DXGI_SWAP_CHAIN_DESC{ 0 };
ID3DBlob* DxHandler::vertexShaderBuffer = nullptr;
ID3DBlob* DxHandler::pixelShaderBuffer = nullptr;

ID3D11PixelShader* DxHandler::pixelPtr = nullptr;
ID3D11VertexShader* DxHandler::vertexPtr = nullptr;

ID3D11VertexShader* DxHandler::lightVertexPtr = nullptr;
ID3D11PixelShader* DxHandler::lightPixelPtr = nullptr;
ID3DBlob* DxHandler::deferredVertexShaderBuffer = nullptr;
ID3DBlob* DxHandler::deferredPixelShaderBuffer = nullptr;

ID3D11InputLayout* DxHandler::input_layout_ptr = nullptr;

ID3D11DepthStencilView* DxHandler::depthStencil = nullptr;
ID3D11Texture2D* DxHandler::depthBuffer = nullptr;

ID3D11Buffer* DxHandler::PSConstBuff;

ID3D11Texture2D* DxHandler::deferredDepthStencilBuffer;
ID3D11DepthStencilView* DxHandler::deferredDepthStencilView;
D3D11_VIEWPORT DxHandler::deferredViewport;

ID3D11SamplerState* DxHandler::deferredSamplerState;

DeferredRenderBuffer* DxHandler::deferredVertexPositionBuffer;
DeferredRenderBuffer* DxHandler::deferredNormalBuffer;

ID3D11InputLayout* DxHandler::deferred_input_layout_ptr;

DxHandler::~DxHandler()
{
	for (int i = 0; i < loadedVSBuffers.size(); i++)
	{
		loadedVSBuffers.at(i)->Release();
	}

	for (int i = 0; i < loadedPSBuffers.size(); i++)
	{
		loadedPSBuffers.at(i)->Release();
	}

	if (deferredNormalBuffer)
		delete deferredNormalBuffer;

	if (deferredVertexPositionBuffer)
		delete deferredVertexPositionBuffer;
}

ID3D11Buffer* DxHandler::createVSConstBuffer(VS_CONSTANT_MATRIX_BUFFER& matrix)
{
	//VS_CONSTANT_MATRIX_BUFFER cBuffer;

	D3D11_BUFFER_DESC constBDesc;
	constBDesc.ByteWidth = sizeof(matrix);
	constBDesc.Usage = D3D11_USAGE_DEFAULT;
	constBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBDesc.CPUAccessFlags = 0;
	constBDesc.MiscFlags = 0;
	constBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &matrix;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	ID3D11Buffer* constantBuffer = NULL;
	HRESULT buffSucc = devicePtr->CreateBuffer(&constBDesc, &InitData,
		&constantBuffer);
	assert(SUCCEEDED(buffSucc));

	//deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	DxHandler::contextPtr->VSSetConstantBuffers(PER_OBJECT_CBUFFER_SLOT, 1, &constantBuffer);

	loadedVSBuffers.push_back(constantBuffer);

	return constantBuffer;
}

ID3D11Buffer*& DxHandler::createPSConstBuffer(PS_CONSTANT_LIGHT_BUFFER &matrix)
{
	D3D11_BUFFER_DESC constPixelDesc;
	constPixelDesc.ByteWidth = sizeof(PS_CONSTANT_LIGHT_BUFFER);
	constPixelDesc.Usage = D3D11_USAGE_DEFAULT;
	constPixelDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constPixelDesc.CPUAccessFlags = 0;
	constPixelDesc.MiscFlags = 0;
	constPixelDesc.StructureByteStride = 0;

	//PS_CONSTANT_LIGHT_BUFFER cPixelBuffer;

	D3D11_SUBRESOURCE_DATA InitDataPixel;
	InitDataPixel.pSysMem = &matrix;
	InitDataPixel.SysMemPitch = 0;
	InitDataPixel.SysMemSlicePitch = 0;
	ID3D11Buffer* constantPixelBuffer = NULL;
	HRESULT buffPSucc = devicePtr->CreateBuffer(&constPixelDesc, &InitDataPixel,
		&constantPixelBuffer);
	assert(SUCCEEDED(buffPSucc));

	//cPixelBuffer.light = DirectX::XMFLOAT4(0, 0, -3, 0);
	contextPtr->UpdateSubresource(constantPixelBuffer, 0, NULL, &matrix, 0, 0);
	contextPtr->PSSetConstantBuffers(0, 1, &constantPixelBuffer);

	loadedPSBuffers.push_back(constantPixelBuffer);

	return constantPixelBuffer;
}

void DxHandler::initalizeDeviceContextAndSwapChain()
{
	//ID3D11DeviceContext* contextPtr = contextPtr;
	//ID3D11Device* devicePtr = devicePtr;

	HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DxHandler::swapDesc,
		&DxHandler::swapChainPtr,
		&DxHandler::devicePtr,
		nullptr,
		&DxHandler::contextPtr
	);

	assert(SUCCEEDED(succ));
	assert(devicePtr != nullptr);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	ID3D11RasterizerState* rasterizerState;
	
	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	contextPtr->RSSetState(rasterizerState);

	wrl::ComPtr<ID3D11Texture2D> backBufferPtr;// not sure how this works but backbufferptr wont work with normal ID3D11ResourcePtr
	(swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferPtr));
	HRESULT rendertargetSucc = devicePtr->CreateRenderTargetView(backBufferPtr.Get(), nullptr, &renderTargetPtr);
	assert(SUCCEEDED(rendertargetSucc));
}

void DxHandler::configureSwapChain(HWND& hWnd)
{
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE; //The hardware driver we'd like to use.
	D3D_FEATURE_LEVEL version[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 }; //Which version we're running
	// CREATING OUR CHAIN DESC
	
	//BUFFERDESC
	DxHandler::swapDesc.BufferDesc.Width = 0;//widthOfScreen;
	DxHandler::swapDesc.BufferDesc.Height = 0;//heightOfScreen;
	DxHandler::swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// doesnt work with 32 for some reason
	DxHandler::swapDesc.BufferDesc.RefreshRate.Numerator = 60;//can be set to 0 for no max
	DxHandler::swapDesc.BufferDesc.RefreshRate.Denominator = 0;
	DxHandler::swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DxHandler::swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//SAMPLEDESC
	DxHandler::swapDesc.SampleDesc.Count = 1;
	DxHandler::swapDesc.SampleDesc.Quality = 0;
	//END SAMPLEDESC

	//GENERAL SWAPDESC
	DxHandler::swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DxHandler::swapDesc.BufferCount = 2; // might want 1 here. Not sure
	DxHandler::swapDesc.OutputWindow = hWnd;
	DxHandler::swapDesc.Windowed = TRUE;
	DxHandler::swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DxHandler::swapDesc.Flags = 0;
		//Checks for debug mode or not/////////
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//END GENERAL SWAPDESC

	/*HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DxHandler::swapDesc,
		&DxHandler::swapChainPtr,
		&DxHandler::devicePtr,
		nullptr,
		&DxHandler::contextPtr
	);
		std::string s = std::to_string(succ);
	//MessageBox(NULL, LPCWSTR(s.c_str()), L"Blah blah", MB_YESNO);//notice the L is removed

	assert(succ == S_OK);*/
	//assert(contextPtr != nullptr);
	//assert(swapChainPtr != nullptr);
	////

}
void DxHandler::setupInputLayout() //Sets up vertex data layout.
{
	//ID3D11InputLayout* input_layout_ptr = NULL;

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT inputLayoutSucc = devicePtr->CreateInputLayout
	(
		inputDesc, 
		ARRAYSIZE(inputDesc), 
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&input_layout_ptr
	);
	assert(SUCCEEDED(inputLayoutSucc));

	//Deferred magic unicorn input layout
	//The layout for reading from the g-buffers (textures)
	D3D11_INPUT_ELEMENT_DESC deferredInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	HRESULT deferredInputLayoutSucc = devicePtr->CreateInputLayout
	(
		inputDesc,
		ARRAYSIZE(inputDesc),
		deferredVertexShaderBuffer->GetBufferPointer(),
		deferredVertexShaderBuffer->GetBufferSize(),
		&deferred_input_layout_ptr
	);

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	deferredVertexShaderBuffer->Release();
	deferredVertexShaderBuffer = nullptr;

	deferredPixelShaderBuffer->Release();
	deferredPixelShaderBuffer = nullptr;
}
ID3D11Buffer* DxHandler::createVertexBuffer(Mesh& mesh)
{
	ID3D11Buffer* vertexBufferPtr = NULL;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = sizeof(float) * FLOATS_PER_VERTEX * mesh.vertices.size();
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResData = D3D11_SUBRESOURCE_DATA{0};
	subResData.pSysMem = mesh.vertices.data();
	subResData.SysMemPitch = FLOATS_PER_VERTEX * sizeof(float) * mesh.vertices.size();

	MSG msg;

	HRESULT succ2 = devicePtr->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(succ2 == S_OK);

	mesh.vertexBuffer = vertexBufferPtr;
	
	return vertexBufferPtr;
}
ID3D11Buffer* DxHandler::createIndexBuffer(Mesh& mesh)
{
	ID3D11Buffer* indexBuff;

	D3D11_BUFFER_DESC indexBDesc{ 0 };
	indexBDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBDesc.ByteWidth = sizeof(DWORD) * mesh.indicies.size(); //???
	indexBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = mesh.indicies.data();
	HRESULT bigBuffSucc = devicePtr->CreateBuffer(&indexBDesc, &initData, &indexBuff);
	assert(SUCCEEDED(bigBuffSucc));

	mesh.indexBuffer = indexBuff;

	return indexBuff;
}
void DxHandler::setCullingMode(D3D11_CULL_MODE mode)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = mode;
	ID3D11RasterizerState* rasterizerState;

	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	contextPtr->RSSetState(rasterizerState);
}	

void DxHandler::setupPShader(const wchar_t fileName[])
{
	ID3DBlob* errorMessage;

	HRESULT pixelShaderSucc = D3DCompileFromFile(
		fileName,
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"ps_5_0", //Pixel shader
		0,
		0,
		&DxHandler::pixelShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(pixelShaderSucc));

	HRESULT createPShaderSucc = devicePtr->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelPtr);
	assert(SUCCEEDED(createPShaderSucc));
}

void DxHandler::setupVShader(const wchar_t fileName[])
{
	ID3DBlob* errorMessage;
	
	HRESULT	vertShaderSucc = D3DCompileFromFile(
		fileName,
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"vs_5_0", //Vertex shader
		0,
		0,
		&DxHandler::vertexShaderBuffer,
		&errorMessage
	);

	HRESULT createVShaderSucc = devicePtr->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

void DxHandler::setupDepthBuffer(int widthOfRenderWindow, int heightOfRenderWindow)
{
	D3D11_TEXTURE2D_DESC depthDesc{ 0 };

	depthDesc.Width = widthOfRenderWindow;
	depthDesc.Height = heightOfRenderWindow;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	devicePtr->CreateTexture2D(&depthDesc, NULL, &depthBuffer);
	devicePtr->CreateDepthStencilView(DxHandler::depthBuffer, NULL, &depthStencil);

	//depthStencil->Release();
	//depthBuffer->Release();
}

void DxHandler::setupLightBuffer()
{
	//Init light
	PS_CONSTANT_LIGHT_BUFFER lightBuff;
	PSConstBuff = this->createPSConstBuffer(lightBuff);
	DxHandler::contextPtr->UpdateSubresource(PSConstBuff, 0, NULL, &lightBuff, 0, 0);
}

void DxHandler::setupDeferredBuffers(int width, int height)
{
	D3D11_TEXTURE2D_DESC depthBuffDesc{ 0 };

	deferredNormalBuffer = new DeferredRenderBuffer;
	deferredVertexPositionBuffer = new DeferredRenderBuffer;

	//DeferredRenderBuffer deferredRenderNormals(width, height);
	//DeferredRenderBuffer deferredRenderPositions(width, height);
	deferredNormalBuffer->init(width, height);
	deferredVertexPositionBuffer->init(width, height);

	depthBuffDesc.Width = width;
	depthBuffDesc.Height = height;

	depthBuffDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBuffDesc.SampleDesc.Count = 1;
	depthBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBuffDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBuffDesc.MipLevels = 1;
	depthBuffDesc.ArraySize = 1;

	HRESULT depthTextureCreateSucc = DxHandler::devicePtr->CreateTexture2D(&depthBuffDesc, NULL, &deferredDepthStencilBuffer);
	assert(SUCCEEDED(depthTextureCreateSucc));
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;
	depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthViewDesc.Flags = 0;


	HRESULT depthStencilSucc = DxHandler::devicePtr->CreateDepthStencilView(deferredDepthStencilBuffer, &depthViewDesc, &deferredDepthStencilView);
	assert(SUCCEEDED(depthStencilSucc));

}

void DxHandler::setupFirstPassDeferredShaders()
{
	//Pixel shader
	ID3DBlob* errorMessage;
	HRESULT pixelShaderSucc = D3DCompileFromFile(
		L"./DeferredPixel.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"ps_5_0", //Pixel shader
		0,
		0,
		&DxHandler::pixelShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(pixelShaderSucc));

	HRESULT createPShaderSucc = devicePtr->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelPtr);
	assert(SUCCEEDED(createPShaderSucc));

	//Vertex shader
	HRESULT	vertShaderSucc = D3DCompileFromFile(
		L"./DeferredVertex.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"vs_5_0", //Vertex shader
		0,
		0,
		&DxHandler::vertexShaderBuffer,
		&errorMessage
	);

	HRESULT createVShaderSucc = devicePtr->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

void DxHandler::setupSecondPassDeferredShaders()
{
	//Post processing shaders that read from the first pass shader buffers

	//Pixel shader
	ID3DBlob* errorMessage;
	HRESULT pixelShaderSucc = D3DCompileFromFile(
		L"./DeferredLightPixel.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"ps_5_0", //Pixel shader
		0,
		0,
		&DxHandler::deferredPixelShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(pixelShaderSucc));

	HRESULT createPShaderSucc = devicePtr->CreatePixelShader(deferredPixelShaderBuffer->GetBufferPointer(), deferredPixelShaderBuffer->GetBufferSize(), NULL, &lightPixelPtr);
	assert(SUCCEEDED(createPShaderSucc));

	//Vertex shader
	HRESULT	vertShaderSucc = D3DCompileFromFile(
		L"./DeferredLightVertex.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"vs_5_0", //Vertex shader
		0,
		0,
		&DxHandler::deferredVertexShaderBuffer,
		&errorMessage
	);

	HRESULT createVShaderSucc = devicePtr->CreateVertexShader(deferredVertexShaderBuffer->GetBufferPointer(), deferredVertexShaderBuffer->GetBufferSize(), NULL, &lightVertexPtr);
	assert(SUCCEEDED(createVShaderSucc));

	D3D11_SAMPLER_DESC samDesc;
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MaxAnisotropy = 1;
	//samDesc.BorderColor[4] = { 0 };
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samDesc.MinLOD = 0;
	samDesc.MipLODBias = 0.0f;
	HRESULT samplerSucc = devicePtr->CreateSamplerState(&samDesc, &deferredSamplerState);
	assert(SUCCEEDED(samplerSucc));
}

void DxHandler::draw(EngineObject& drawObject)
{

	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	for (int i = 0; i < drawObject.meshes.size(); i++)
	{
		DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawObject.meshes.at(i).vertexBuffer,
			&stride, &offset);

		VS_CONSTANT_MATRIX_BUFFER matrixBuff;
		matrixBuff.worldViewProjectionMatrix = drawObject.meshes.at(i).worldMatrix * Camera::cameraView * Camera::cameraProjectionMatrix;
		matrixBuff.worldMatrix = drawObject.meshes.at(i).worldMatrix;
		//matrixBuff.worldViewProjectionMatrix = Camera::cameraProjectionMatrix * Camera::cameraView * drawObject.meshes.at(i).worldMatrix;
		//DirectX::XMMatrixTranspose(matrixBuff.worldViewProjectionMatrix);

		DxHandler::contextPtr->UpdateSubresource(this->loadedVSBuffers[PER_OBJECT_CBUFFER_SLOT], 0, NULL, & matrixBuff, 0, 0);

		//contextPtr->PSSetShaderResources(0, 1, &drawObject.textureView); Normal texture
		//-------------------------------------------------------
		

		//Update light stuff
		PS_CONSTANT_LIGHT_BUFFER lightBuff;
		lightBuff.lightPos = DirectX::XMVectorSet(0, 0, -3, 0);
		lightBuff.ambientMeshColor = drawObject.meshes.at(i).ambientMeshColor;
		lightBuff.diffueMeshColor = drawObject.meshes.at(i).diffueMeshColor;
		lightBuff.specularMeshColor = drawObject.meshes.at(i).specularMeshColor;
		lightBuff.worldViewProjectionMatrix = matrixBuff.worldViewProjectionMatrix;
		lightBuff.camPos = Camera::cameraPosition;
		lightBuff.specularExponent = DirectX::XMVectorSet(drawObject.meshes.at(i).specularExponent, 0, 0, 0);

		DxHandler::contextPtr->UpdateSubresource(PSConstBuff, 0, NULL, &lightBuff, 0, 0);
		DxHandler::contextPtr->Draw(drawObject.meshes.at(i).vertices.size(), 0);

		//float clearColor[4] = { 0.5,0.5,0,0 };
		//RESET BACK TO ORIGINAL RENDER BUFFER
		//contextPtr->ClearRenderTargetView(renderTargetPtr, clearColor);
		//contextPtr->OMSetRenderTargets(1, &renderTargetPtr, NULL);
	}
	
	//swapChainPtr->Present(1, 0);
}

void DxHandler::drawIndexedMesh(EngineObject& drawObject)
{
	/*
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	for (int i = 0; i < drawObject.meshes.size(); i++)
	{
		DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawObject.meshes.at(i).vertexBuffer,
			&stride, &offset);
		DxHandler::contextPtr->IASetIndexBuffer(drawObject.meshes.at(i).indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		VS_CONSTANT_MATRIX_BUFFER matrixBuff;
		//matrixBuff.scaleMatrix = drawObject.meshes.at(i).scalingMatrix;
		//matrixBuff.translationMatrix = drawObject.meshes.at(i).translationMatrix;
		//matrixBuff.rotationMatrix = drawObject.meshes.at(i).rotationMatrix;
		
		matrixBuff.worldViewProjectionMatrix = drawObject.meshes.at(i).worldMatrix * Camera::cameraView * Camera::cameraProjectionMatrix;
		DirectX::XMMatrixTranspose(matrixBuff.worldViewProjectionMatrix);

		DxHandler::contextPtr->UpdateSubresource(this->loadedVSBuffers[PER_OBJECT_CBUFFER_SLOT], 0, NULL, &matrixBuff, 0, 0);

		contextPtr->PSSetShaderResources(0, 1, &drawObject.textureView);
		DxHandler::contextPtr->DrawIndexed(drawObject.meshes.at(i).indicies.size(), 0, 0);
	}
	*/
}
