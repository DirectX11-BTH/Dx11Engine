﻿#include "DXHandler.h"

ID3D11Device* DxHandler::devicePtr = nullptr;
ID3D11DeviceContext* DxHandler::contextPtr = nullptr;
HINSTANCE DxHandler::hInstance = HINSTANCE();

IDXGISwapChain* DxHandler::swapChainPtr = nullptr;
ID3D11RenderTargetView* DxHandler::renderTargetPtr = nullptr;
DXGI_SWAP_CHAIN_DESC DxHandler::swapDesc = DXGI_SWAP_CHAIN_DESC{ 0 };
ID3DBlob* DxHandler::vertexShaderBuffer = nullptr;
ID3DBlob* DxHandler::pixelShaderBuffer = nullptr;

ID3D11Texture2D* DxHandler::gaussianTexture = nullptr;
ID3D11ShaderResourceView* DxHandler::gaussianSRV = nullptr;

float DxHandler::WIDTH;
float DxHandler::HEIGHT;


ID3DBlob* DxHandler::deferredPixelShaderBuffer = nullptr;
ID3DBlob* DxHandler::deferredVertexShaderBuffer = nullptr;

ID3DBlob* DxHandler::geometryShaderBuffer = nullptr;
ID3D11GeometryShader* DxHandler::geometryPtr = nullptr;

ID3D11ComputeShader* DxHandler::computeShaderPtr = nullptr;
ID3DBlob* DxHandler::computeShaderBuffer = nullptr;
ID3DBlob* DxHandler::csErrorBlob = nullptr;

ID3D11PixelShader* DxHandler::deferredPixelPtr = nullptr;
ID3D11VertexShader* DxHandler::deferredVertexPtr = nullptr;
ID3D11PixelShader* DxHandler::pixelPtr = nullptr;
ID3D11VertexShader* DxHandler::vertexPtr = nullptr;
ID3D11InputLayout* DxHandler::input_layout_ptr = nullptr;

ID3D11DepthStencilView* DxHandler::depthStencil = nullptr;
ID3D11Texture2D* DxHandler::depthBuffer = nullptr;

HWND* DxHandler::hWnd = nullptr;

ID3D11Buffer* DxHandler::PSConstBuff;
ID3D11Buffer* DxHandler::GSConstBuff;

Mesh* DxHandler::fullscreenQuad = nullptr;

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
}

ID3D11Texture2D* DxHandler::generateGaussianKernel() //Need to dynamically generate this
{
	std::vector<DirectX::XMFLOAT4> gaussianArr;
	//DirectX::XMFLOAT4* gaussianArr; 
	//gaussianArr = new DirectX::XMFLOAT4[15*15], 15;

	int kernelSize = 25;
	float sigma = 30.f; //How intensive blur is

	float PI = 3.14159265359;
	float sum = 0;

	for (int y = 0; y < kernelSize; y++)
	{
		for (int x = 0; x < kernelSize; x++)
		{
			float xDist = abs(floor(x - kernelSize / 2)+1);
			float yDist = abs(floor(y - kernelSize / 2)+1);
			float val = exp(-(xDist * xDist + yDist * yDist) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
			sum += val;

			gaussianArr.push_back(DirectX::XMFLOAT4(val, 0, 0, 0));
			//gaussianArr[x + y * kernelSize] = DirectX::XMFLOAT4(val, 0, 0, 0);
		}
	}

	//normalize
	for (int y = 0; y < kernelSize; y++)
	{
		for (int x = 0; x < kernelSize; x++)
		{
			gaussianArr[x + y * kernelSize] = DirectX::XMFLOAT4(gaussianArr[x + y * kernelSize].x / sum, 0, 0, 0);//sum;//DirectX::XMVectorSet(DirectX::XMVectorGetX(gaussianArr[x + y * kernelSize]) / sum, 0, 0, 0);
			//gaussianArr[x + y * kernelSize] = DirectX::XMFLOAT4(1, 0, 0, 1);
		}
	}

	return textureFromGaussian(gaussianArr, kernelSize);
}

ID3D11Texture2D* DxHandler::textureFromGaussian(std::vector<DirectX::XMFLOAT4>& gaussianArr, int kernelSize)
{
	ID3D11Texture2D* returnTexture;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = gaussianArr.data();
	data.SysMemPitch = sizeof(DirectX::XMFLOAT4) * kernelSize;//sizeof(float)*4*kernelSize; //Size per line, 4 because 4 floats per entry and then times per kernelSize.
	data.SysMemSlicePitch = 0;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = kernelSize;
	textureDesc.Height = kernelSize;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	HRESULT texSucc = devicePtr->CreateTexture2D(&textureDesc, &data, &returnTexture);
	assert(SUCCEEDED(texSucc));

	SRVFromGaussian(returnTexture, &textureDesc);
	return returnTexture;
}

ID3D11ShaderResourceView* DxHandler::SRVFromGaussian(ID3D11Texture2D* texture, D3D11_TEXTURE2D_DESC* texDesc)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;

	shaderResourceDesc.Format = texDesc->Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	HRESULT shaderResourceSucc = DxHandler::devicePtr->CreateShaderResourceView(texture, &shaderResourceDesc, &gaussianSRV);
	assert(SUCCEEDED(shaderResourceSucc));

	//gaussianSRV = gaussianSRV;

	return gaussianSRV;
}

ID3D11Texture2D* DxHandler::blurTexture(ID3D11Texture2D* readTexture, ID3D11ShaderResourceView* readSRV)
{
	//make the magic shit happen
	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;

	ID3D11Texture2D* returnTexture;
	ID3D11UnorderedAccessView* uav;

	int texWidth = 720;
	int texHeight = 720;
	//TEXTURE DESC
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //RGBA 4 lyf
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	
	HRESULT texSucc = DxHandler::devicePtr->CreateTexture2D(&texDesc, NULL, &returnTexture);
	assert(SUCCEEDED(texSucc));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	HRESULT succ = DxHandler::devicePtr->CreateUnorderedAccessView(returnTexture, &uavDesc, &uav);
	assert(SUCCEEDED(succ));

	//Try magical compute shader - DO NOT DELETE ------------------------------------
	DxHandler::contextPtr->CSSetShader(DxHandler::computeShaderPtr, NULL, 0);
	DxHandler::contextPtr->CSSetShaderResources(0, 1, &readSRV); //Read from blur
	DxHandler::contextPtr->CSSetShaderResources(1, 1, &gaussianSRV); //Gaussian Kernel generated on CPU
	//DxHandler::contextPtr->CSSetShaderResources(1, 1, &gBuffHandler.buffers[GBufferType::DiffuseColor].shaderResourceView); //Write to

	DxHandler::contextPtr->CSSetUnorderedAccessViews(0, 1, &uav, NULL);
	DxHandler::contextPtr->Dispatch(40, 40, 1);

	DxHandler::contextPtr->CSSetUnorderedAccessViews(0, 1, nullUAV, NULL);
	DxHandler::contextPtr->CSSetShaderResources(0, 1, nullSRV); //Read from blur
	DxHandler::contextPtr->CSSetShader(NULL, NULL, 0);
	//Try magical compute shader ----------------------------------------------------

	//contextPtr->UpdateSubresource(readTexture, NULL, NULL, returnTexture, sizeof(float)*4*texWidth, 0);
	contextPtr->CopyResource(readTexture, returnTexture);

	
	returnTexture->Release();
	uav->Release();
	return returnTexture;
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

ID3D11Buffer* DxHandler::createVSConstBuffer(VS_CONSTANT_CAMERA_BUFFER& matrix)
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
	DxHandler::contextPtr->VSSetConstantBuffers(CAMERA_CBUFFER_SLOT, 1, &constantBuffer);

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

ID3D11Buffer*& DxHandler::createGSConstBuffer()
{
	D3D11_BUFFER_DESC constGeometryDesc;
	constGeometryDesc.ByteWidth = sizeof(GS_CONSTANT_MATRIX_BUFFER);
	constGeometryDesc.Usage = D3D11_USAGE_DEFAULT;
	
	constGeometryDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constGeometryDesc.CPUAccessFlags = 0;
	constGeometryDesc.MiscFlags = 0;
	constGeometryDesc.StructureByteStride = 0;

	ID3D11Buffer* constantGeometryBuffer = NULL;
	HRESULT buffPSucc = devicePtr->CreateBuffer(&constGeometryDesc, NULL,
		&constantGeometryBuffer);
	assert(SUCCEEDED(buffPSucc));

	//cGeometryBuffer.light = DirectX::XMFLOAT4(0, 0, -3, 0);
	//contextPtr->UpdateSubresource(constantGeometryBuffer, 0, NULL, NULL, 0, 0);
	contextPtr->GSSetConstantBuffers(0, 1, &constantGeometryBuffer);

	//loadedPSBuffers.push_back(constantGeometryBuffer);
	GSConstBuff = constantGeometryBuffer;

	return constantGeometryBuffer;
}

void DxHandler::initalizeDeviceContextAndSwapChain()
{
	//ID3D11DeviceContext* contextPtr = contextPtr;
	//ID3D11Device* devicePtr = devicePtr;

	//IDXGIFactory1::EnumAdapters
	IDXGIFactory6* factory;
	IDXGIAdapter* adapter;

	/*for (
		UINT adapterIndex = 0;
		DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(
			adapterIndex,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&adapter->ReleaseAndGetAddressOf()));
		adapterIndex++)*/

	HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0, // D3D11_CREATE_DEVICE_DEBUG
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
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
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
void DxHandler::setupInputLayout()
{
	//ID3D11InputLayout* input_layout_ptr = NULL;

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT inputLayoutSucc = devicePtr->CreateInputLayout
	(
		inputDesc, ARRAYSIZE(inputDesc),
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&input_layout_ptr
	);
	assert(SUCCEEDED(inputLayoutSucc));
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
	//bufferDesc.StructureByteStride = sizeof(float) * FLOATS_PER_VERTEX;

	D3D11_SUBRESOURCE_DATA subResData = D3D11_SUBRESOURCE_DATA{0};
	subResData.pSysMem = mesh.vertices.data();
	subResData.SysMemPitch = FLOATS_PER_VERTEX * sizeof(float) * mesh.vertices.size();

	HRESULT succ2 = devicePtr->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(SUCCEEDED(succ2));

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
	assert(SUCCEEDED(vertShaderSucc));

	HRESULT createVShaderSucc = devicePtr->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

void DxHandler::setupGShader(const wchar_t fileName[])
{
	//Pixel
	ID3DBlob* errorMessage;

	HRESULT GeometryShaderSucc = D3DCompileFromFile(
		L"GeometryShader.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"gs_5_0", //Geometry shader
		0,
		0,
		&geometryShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(GeometryShaderSucc));

	HRESULT createGeometryShaderSucc = devicePtr->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &geometryPtr);
	assert(SUCCEEDED(createGeometryShaderSucc));

	DxHandler::contextPtr->GSSetShader(DxHandler::geometryPtr, NULL, NULL);
}

void DxHandler::setupDeferredShaders()
{
	//Pixel
	ID3DBlob* errorMessage;

	HRESULT deferredPixelShaderSucc = D3DCompileFromFile(
		L"DeferredLightPixel.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"ps_5_0", //Pixel shader
		0,
		0,
		&deferredPixelShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(deferredPixelShaderSucc));

	HRESULT createDeferredPixelShaderSucc = devicePtr->CreatePixelShader(deferredPixelShaderBuffer->GetBufferPointer(), deferredPixelShaderBuffer->GetBufferSize(), NULL, &deferredPixelPtr);
	assert(SUCCEEDED(createDeferredPixelShaderSucc));
	//=============================================
	//Vertex

	HRESULT	deferredVertexShaderSucc = D3DCompileFromFile(
		L"DeferredLightVertex.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"vs_5_0", //Vertex shader
		0,
		0,
		&deferredVertexShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(deferredVertexShaderSucc));

	HRESULT createDeferredVertexShaderSucc = devicePtr->CreateVertexShader(deferredVertexShaderBuffer->GetBufferPointer(), deferredVertexShaderBuffer->GetBufferSize(), NULL, &deferredVertexPtr);
	assert(SUCCEEDED(createDeferredVertexShaderSucc));
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

void DxHandler::draw(EngineObject& drawObject, bool environmentMapping, bool isWater)
{

	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	for (int i = 0; i < drawObject.meshes.size(); i++)
	{
		DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawObject.meshes.at(i).vertexBuffer,
			&stride, &offset);

		VS_CONSTANT_MATRIX_BUFFER matrixBuff;
		//matrixBuff.scaleMatrix = drawObject.meshes.at(i).scalingMatrix;
		//matrixBuff.translationMatrix = drawObject.meshes.at(i).translationMatrix;
		//matrixBuff.rotationMatrix = drawObject.meshes.at(i).rotationMatrix;
		
		matrixBuff.worldViewProjectionMatrix = drawObject.meshes.at(i).worldMatrix * Camera::cameraView * Camera::cameraProjectionMatrix;
		matrixBuff.worldMatrix = drawObject.meshes.at(i).worldMatrix;
		matrixBuff.viewMatrix = Camera::cameraView;
		matrixBuff.projMatrix = Camera::cameraProjectionMatrix;

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(Camera::cameraView);
		matrixBuff.viewInverseMatrix = DirectX::XMMatrixInverse(&det, Camera::cameraView);
		det = DirectX::XMMatrixDeterminant(drawObject.meshes.at(i).worldMatrix);
		matrixBuff.worldInverseMatrix = DirectX::XMMatrixInverse(&det, drawObject.meshes.at(i).worldMatrix);

		DxHandler::contextPtr->UpdateSubresource(this->loadedVSBuffers[PER_OBJECT_CBUFFER_SLOT], 0, NULL, &matrixBuff, 0, 0);
		contextPtr->PSSetShaderResources(0, 1, &drawObject.textureView);
		contextPtr->PSSetShaderResources(1, 1, &drawObject.normalMapContainer.textureView);

		//Update light stuff
		
		lightBuff.lightPos = DirectX::XMVectorSet(300, 300, 300, 1);
		lightBuff.ambientMeshColor = drawObject.meshes.at(i).ambientMeshColor;
		lightBuff.diffuseMeshColor = drawObject.meshes.at(i).diffuseMeshColor;
		lightBuff.specularMeshColor = drawObject.meshes.at(i).specularMeshColor;
		lightBuff.worldViewProjectionMatrix = matrixBuff.worldViewProjectionMatrix;
		lightBuff.camPos = Camera::cameraPosition;
		lightBuff.specularExponent = DirectX::XMVectorSet(drawObject.meshes.at(i).specularExponent, 0, 0, 0);

		det = DirectX::XMMatrixDeterminant(Camera::cameraView);
		lightBuff.viewInverseMatrix = DirectX::XMMatrixInverse(&det, Camera::cameraView);
		det = DirectX::XMMatrixDeterminant(drawObject.meshes.at(i).worldMatrix);
		lightBuff.worldInverseMatrix = DirectX::XMMatrixInverse(&det, drawObject.meshes.at(i).worldMatrix);
		lightBuff.hasTexture = (drawObject.hasTexture);
		lightBuff.hasNormalMap = (drawObject.hasNormalMap);
		lightBuff.environmentMap = environmentMapping;

		lightBuff.worldMatrix = drawObject.meshes.at(i).worldMatrix;
		lightBuff.viewMatrix = Camera::cameraView;
		lightBuff.projMatrix = Camera::cameraProjectionMatrix;
		lightBuff.isWater = isWater;
		lightBuff.glowingObject = drawObject.glowingObject;
		if (isWater)
			lightBuff.uvDisplacement = DirectX::XMFLOAT4(lightBuff.uvDisplacement.x += 0.0001, lightBuff.uvDisplacement.y += 0.0001, 0, 0);
		DxHandler::contextPtr->UpdateSubresource(PSConstBuff, 0, NULL, &lightBuff, 0, 0);

		GS_CONSTANT_MATRIX_BUFFER geometryBuff;
		geometryBuff.camPos = Camera::cameraPosition;
		DxHandler::contextPtr->UpdateSubresource(GSConstBuff, 0, NULL, &geometryBuff, 0, 0);

		DxHandler::contextPtr->Draw(drawObject.meshes.at(i).vertices.size(), 0);
	}
}

void DxHandler::draw(cubeCamera& cubeCam, EngineObject& drawObject, bool isWater)
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	for (int i = 0; i < drawObject.meshes.size(); i++)
	{
		DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawObject.meshes.at(i).vertexBuffer,
			&stride, &offset);

		VS_CONSTANT_MATRIX_BUFFER matrixBuff;
		//matrixBuff.scaleMatrix = drawObject.meshes.at(i).scalingMatrix;
		//matrixBuff.translationMatrix = drawObject.meshes.at(i).translationMatrix;
		//matrixBuff.rotationMatrix = drawObject.meshes.at(i).rotationMatrix;

		matrixBuff.worldViewProjectionMatrix = drawObject.meshes.at(i).worldMatrix * cubeCam.cameraView * cubeCam.cameraProjectionMatrix;
		matrixBuff.worldMatrix = drawObject.meshes.at(i).worldMatrix;
		matrixBuff.viewMatrix = cubeCam.cameraView;//Camera::cameraView;
		matrixBuff.projMatrix = cubeCam.cameraProjectionMatrix;//Camera::cameraProjectionMatrix;

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(cubeCam.cameraView);
		matrixBuff.viewInverseMatrix = DirectX::XMMatrixInverse(&det, cubeCam.cameraView);
		det = DirectX::XMMatrixDeterminant(drawObject.meshes.at(i).worldMatrix);
		matrixBuff.worldInverseMatrix = DirectX::XMMatrixInverse(&det, drawObject.meshes.at(i).worldMatrix);

		DxHandler::contextPtr->UpdateSubresource(this->loadedVSBuffers[PER_OBJECT_CBUFFER_SLOT], 0, NULL, &matrixBuff, 0, 0);
		contextPtr->PSSetShaderResources(0, 1, &drawObject.textureView);
		contextPtr->PSSetShaderResources(1, 1, &drawObject.normalMapContainer.textureView);

		//Update light stuff
		PS_CONSTANT_LIGHT_BUFFER lightBuff;
		lightBuff.lightPos = DirectX::XMVectorSet(300, 300, 300, 1);
		lightBuff.ambientMeshColor = drawObject.meshes.at(i).ambientMeshColor;
		lightBuff.diffuseMeshColor = drawObject.meshes.at(i).diffuseMeshColor;
		lightBuff.specularMeshColor = drawObject.meshes.at(i).specularMeshColor;
		lightBuff.worldViewProjectionMatrix = matrixBuff.worldViewProjectionMatrix;
		lightBuff.camPos = cubeCam.cameraPosition;
		lightBuff.specularExponent = DirectX::XMVectorSet(drawObject.meshes.at(i).specularExponent, 0, 0, 0);

		lightBuff.worldMatrix = drawObject.meshes.at(i).worldMatrix;
		lightBuff.viewMatrix = cubeCam.cameraView;
		lightBuff.projMatrix = cubeCam.cameraProjectionMatrix;

		det = DirectX::XMMatrixDeterminant(cubeCam.cameraView);
		lightBuff.viewInverseMatrix = DirectX::XMMatrixInverse(&det, cubeCam.cameraView);
		det = DirectX::XMMatrixDeterminant(drawObject.meshes.at(i).worldMatrix);
		lightBuff.worldInverseMatrix = DirectX::XMMatrixInverse(&det, drawObject.meshes.at(i).worldMatrix);
		lightBuff.hasTexture = (drawObject.hasTexture);
		lightBuff.hasNormalMap = (drawObject.hasNormalMap);
		lightBuff.isWater = isWater;
		lightBuff.glowingObject = drawObject.glowingObject;
		if (isWater)
			lightBuff.uvDisplacement = DirectX::XMFLOAT4(0, 0, 0, 0);

		DxHandler::contextPtr->UpdateSubresource(PSConstBuff, 0, NULL, &lightBuff, 0, 0);
		DxHandler::contextPtr->Draw(drawObject.meshes.at(i).vertices.size(), 0);

		GS_CONSTANT_MATRIX_BUFFER geometryBuff;
		geometryBuff.camPos = cubeCam.cameraPosition;
		DxHandler::contextPtr->UpdateSubresource(GSConstBuff, 0, NULL, &geometryBuff, 0, 0);
	}
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

void DxHandler::drawFullscreenQuad()
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	DxHandler::contextPtr->IASetVertexBuffers(0, 1, &fullscreenQuad->vertexBuffer,
		&stride, &offset);

	DxHandler::contextPtr->Draw(fullscreenQuad->vertices.size(), 0);
}

void DxHandler::setupComputeShader()
{
	UINT flags = D3DCOMPILE_DEBUG;
	LPCSTR profile = "cs_5_0";

	HRESULT csCompileSucc = D3DCompileFromFile(
		L"GaussianComputeShader.hlsl",
		NULL,
		NULL,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		profile,
		flags, 0,
		&DxHandler::computeShaderBuffer,
		&csErrorBlob
	);
	assert(SUCCEEDED(csCompileSucc));
	
	HRESULT csCreatedSucc = DxHandler::devicePtr->CreateComputeShader(DxHandler::computeShaderBuffer->GetBufferPointer(),
		computeShaderBuffer->GetBufferSize(),
		NULL, &computeShaderPtr);

	assert(SUCCEEDED(csCreatedSucc));

	contextPtr->CSSetShader(DxHandler::computeShaderPtr, NULL, NULL);
}

ID3D11UnorderedAccessView*& DxHandler::textureToUAV(ID3D11Texture2D* texture)
{
	ID3D11UnorderedAccessView* uav;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	HRESULT succ = devicePtr->CreateUnorderedAccessView(texture, &uavDesc, &uav);
	assert(SUCCEEDED(succ));

	return uav;
}

void DxHandler::generateFullscreenQuad()
{
		/*
		float x, y, z = 0;
		float r, g, b, a = 1; //Default to white for debug
		float u, v = 0;
		float nx, ny, nz = 0;☺
		*/

		DxHandler::fullscreenQuad = new Mesh;		//X Y  Z   R	 G  B  A, U, V  nX nY nZ
		fullscreenQuad->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
		fullscreenQuad->vertices.push_back(Vertex{ 1, -1, 0.1f,    1, 1, 1, 1, 1, 1, 0, 0, -1 });
		fullscreenQuad->vertices.push_back(Vertex{ -1,  -1, 0.1f,  1, 1, 1, 1, 0, 1, 0, 0, -1 });

		fullscreenQuad->vertices.push_back(Vertex{ -1,  1, 0.1f,  1, 1, 1, 1, 0, 0, 0, 0, -1 });
		fullscreenQuad->vertices.push_back(Vertex{ 1,  1, 0.1f,   1, 1, 1, 1, 1, 0, 0, 0, -1 });
		fullscreenQuad->vertices.push_back(Vertex{ 1,  -1, 0.1f, 1, 1, 1, 1, 1, 1, 0, 0, -1 });

		createVertexBuffer(*fullscreenQuad);
}
