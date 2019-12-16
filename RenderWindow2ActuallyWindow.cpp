/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/

// Renderwindow.cpp : This file contains the 'main' function. Program execution begins and ends there.
#define WIN32_LEAN_AND_MEAN
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <assert.h>
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <wrl.h>
#include <vector>
#include <stdint.h>
#include <chrono>

#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>
#include <math.h>
#include "DXHandler.h"

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1);
namespace wrl = Microsoft::WRL;
TCHAR szWindowClass[] = _T("Glorious App");
TCHAR szWindowTitle[] = _T("Glorious 3D Renderer");
//global variables for X any Y
int widthOfScreen = 600;
int heightOfScreen = 500;
HWND hWnd = NULL;

int main()
{
	std::cout << "Hello World!\n";
}

//Function handles signals
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//PAINTSTRUCT ps;
	HDC hdc; //HDC is to handle		 context.

	switch (message)
	{
	/*case WM_PAINT: //If the window is resized
		hdc = BeginPaint(hWnd, &ps); //Indicate it's using paint func

		EndPaint(hWnd, &ps); //Return control of the paint func
		break;*/
	case WM_DESTROY: //Destroy window
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN: //If someone clicks left mouse button
		MessageBox(hWnd, L"CLICKED", L"TITLE", MB_OK); //hWnd is which app / window it belongs to
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/*
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	*/
	std::cout << "WinMain was called\n";
	WNDCLASSEX window;
	window.cbSize = sizeof(WNDCLASSEX);
	window.style = CS_HREDRAW | CS_VREDRAW;
	window.lpfnWndProc = WndProc; //The function we want to call when we get an event. Could technically call function WndProc anything.
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	window.hInstance = hInstance;
	window.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window.lpszMenuName = NULL;
	window.lpszClassName = szWindowClass;
	window.hIconSm = LoadIcon(window.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&window))
	{
		MessageBox(NULL,
			_T("Call to registerclassex failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	};
	hWnd = CreateWindow(
		szWindowClass,
		szWindowTitle,
		WS_OVERLAPPEDWINDOW, //Type of window
		CW_USEDEFAULT, CW_USEDEFAULT, //Startpos
		widthOfScreen, heightOfScreen, //Size
		NULL,
		NULL,
		DxHandler::hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to create window failed!"),
			_T("Windows Desktop Guided Tour!"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow); //Actually displays the window
	UpdateWindow(hWnd);

	//---------------------------------------------------------------------------------------------------------- DX11 below

	ID3D11Device* devicePtr;
	IDXGISwapChain* swapChainPtr;
	//IDXGIAdapter* pAdapter = NULL; //The graphics adapter we're using, can be set to NULL to use standard.
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE; //The hardware driver we'd like to use.
	D3D_FEATURE_LEVEL version[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 }; //Which version we're running
	ID3D11RenderTargetView* renderTargetPtr;

	// CREATING OUR CHAIN DESC
	DXGI_SWAP_CHAIN_DESC swapDesc = DXGI_SWAP_CHAIN_DESC{ 0 };
	//BUFFERDESC
	swapDesc.BufferDesc.Width = 0;//widthOfScreen;
	swapDesc.BufferDesc.Height = 0;//heightOfScreen;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// doesnt work with 32 for some reason
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;//can be set to 0 for no max
	swapDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//END BUFFERDESC

	//SAMPLEDESC
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	//END SAMPLEDESC

	//GENERAL SWAPDESC
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 2; // might want 1 here. Not sure
	swapDesc.OutputWindow = hWnd;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = 0;
	//Checks for debug mode or not/////////
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//END GENERAL SWAPDESC

	ID3D11DeviceContext* deviceContext;

	

	HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChainPtr,
		&devicePtr,
		nullptr,
		&deviceContext
	);

	std::string s = std::to_string(succ);
	//MessageBox(NULL, LPCWSTR(s.c_str()), L"Blah blah", MB_YESNO);//notice the L is removed

	assert(succ == S_OK);
	assert(deviceContext != nullptr);
	assert(swapChainPtr != nullptr);

	//	ID3D11Texture2D* pSwapChain = 0;
	wrl::ComPtr<ID3D11Texture2D> backBufferPtr;// not sure how this works but backbufferptr wont work with normal ID3D11ResourcePtr
	(swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferPtr));
	HRESULT rendertargetSucc = devicePtr->CreateRenderTargetView(backBufferPtr.Get(), nullptr, &renderTargetPtr);
	assert(SUCCEEDED(rendertargetSucc));

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	ID3D11RasterizerState* rasterizerState;

	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	deviceContext->RSSetState(rasterizerState);

		//NOW WE CREATE AN ARRAY OF FLOATS AND CALC THE BYTES

	int nrOfVertices = 6;
	int nrOfFloatsPerVertex = 12;
	float fArray[] =
	// (-1,-1) bottom left // UV-scale  (0,0) top left
	{
		//		XYZ		//		//       RGBA     //	//  UV  //	//nX nY nZ//
		-0.7f, 0.7f, 0.5f,		 1.f, 0.f, 0.f, 1.f,	0.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.7f, -0.7f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.f, 1.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		-0.7f, -0.7f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	0.0f, 1.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ

		-0.7f, 0.7f, 0.5f,		 1.f, 0.f, 0.f, 0.f,	0.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.7f, 0.7f, 0.5f,		 0.f, 0.f, 1.f, 1.f,	1.f, 0.f,	0, 0, -1,//XYZ RGBA UV nXnYnZ
		0.7f, -0.7f, 0.5f,		 0.f, 1.f, 0.f, 1.f,	1.0f, 1.f,	0, 0, -1//XYZ RGBA UV nXnYnZ
	};

	int sizeOfBuffer = sizeof(float) * nrOfFloatsPerVertex * nrOfVertices;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = sizeOfBuffer;//80;// must be divisible by 16. Unclear what to put here
	bufferDesc.StructureByteStride = 0;//sizeof(float) * 7;

	D3D11_SUBRESOURCE_DATA subResData = D3D11_SUBRESOURCE_DATA{};
	subResData.pSysMem = fArray;
	subResData.SysMemPitch = 512 * sizeof(float)*4;

	ID3D11Buffer* vertexBufferPtr = NULL;
	MSG msg;

	HRESULT succ2 = devicePtr->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(succ2 == S_OK);

	//SHADER SECTION ------------------------------------
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	ID3DBlob* errorMessage;

	HRESULT pixelShaderSucc = D3DCompileFromFile(
		L"PShader.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"ps_5_0", //Pixel shader
		0,
		0,
		&pixelShaderBuffer,
		&errorMessage
	);

	HRESULT	vertShaderSucc = D3DCompileFromFile(
		L"VShader.hlsl",
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"vs_5_0", //Vertex shader
		0,
		0,
		&vertexShaderBuffer,
		&errorMessage
	);
	assert(vertShaderSucc == S_OK);
	assert(pixelShaderSucc == S_OK);
	assert(vertexShaderBuffer != nullptr);
	assert(pixelShaderBuffer != nullptr);

	ID3D11VertexShader* vertexPtr;
	ID3D11PixelShader* pixelPtr;
	HRESULT createVShaderSucc = devicePtr->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexPtr);
	HRESULT createPShaderSucc = devicePtr->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelPtr);
	assert(SUCCEEDED(createVShaderSucc));
	assert(SUCCEEDED(createPShaderSucc));
	//------------------------------------

	//INPUT LAYOUT----------------------------
	ID3D11InputLayout* input_layout_ptr = NULL;

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT inputLayoutSucc = devicePtr->CreateInputLayout
	(
		inputDesc, ARRAYSIZE(inputDesc),
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&input_layout_ptr
	);
	assert(SUCCEEDED(inputLayoutSucc));
	//--------------------------------------
	//CONSTANT VERTEX BUFFER SETUP

	struct VS_CONSTANT_BUFFER
	{
		DirectX::XMMATRIX rotationMatrix;
	};
	float radToDeg =  3.14159265/ 180;
	float rotationIncrement = 0;
	float rotation = radToDeg * rotationIncrement;
	VS_CONSTANT_BUFFER cBuffer;
	cBuffer.rotationMatrix = DirectX::XMMATRIX
	(
	cos(rotation),	0,		sin(rotation),		0,
	0,				1,		0,					0,
	-sin(rotation),	0,		cos(rotation),		0,
	0,				0,		0,					1
	);

	D3D11_BUFFER_DESC constBDesc;
	constBDesc.ByteWidth = sizeof( VS_CONSTANT_BUFFER );
	constBDesc.Usage = D3D11_USAGE_DEFAULT;
	constBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBDesc.CPUAccessFlags = 0;
	constBDesc.MiscFlags = 0;
	constBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &cBuffer;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	ID3D11Buffer*   constantBuffer = NULL;
	HRESULT buffSucc = devicePtr->CreateBuffer( &constBDesc, &InitData, 
                                 &constantBuffer );
	assert(SUCCEEDED(buffSucc));
	
	deviceContext->VSSetConstantBuffers( 0, 1, &constantBuffer );
	//--------------------------------------

	//CONSTANT PIXEL BUFFER 

	struct PS_CONSTANT_BUFFER
	{
		DirectX::XMFLOAT4 light = DirectX::XMFLOAT4(0, 0.5, -2, 0);
	};

	D3D11_BUFFER_DESC constPixelDesc;
	constPixelDesc.ByteWidth = sizeof(PS_CONSTANT_BUFFER);
	constPixelDesc.Usage = D3D11_USAGE_DEFAULT;
	constPixelDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constPixelDesc.CPUAccessFlags = 0;
	constPixelDesc.MiscFlags = 0;
	constPixelDesc.StructureByteStride = 0;

	PS_CONSTANT_BUFFER cPixelBuffer;

	D3D11_SUBRESOURCE_DATA InitDataPixel;
	InitDataPixel.pSysMem = &cPixelBuffer;
	InitDataPixel.SysMemPitch = 0;
	InitDataPixel.SysMemSlicePitch = 0;
	ID3D11Buffer* constantPixelBuffer = NULL;
	HRESULT buffPSucc = devicePtr->CreateBuffer(&constPixelDesc, &InitDataPixel,
		&constantPixelBuffer);
	assert(SUCCEEDED(buffPSucc));

	cPixelBuffer.light = DirectX::XMFLOAT4(0, 0, -3, 0);
	deviceContext->UpdateSubresource(constantPixelBuffer, 0, NULL, &cPixelBuffer, 0, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &constantPixelBuffer);

	//--------------------------------------

	//TEXTURE MAGIC
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	HRESULT textureSucc = DirectX::CreateWICTextureFromFile(devicePtr, L"texture.png", &texture, &textureView, 0);
	assert(SUCCEEDED(textureSucc));

	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };
	imageSampleDesc.Width = 512;
	imageSampleDesc.Height = 512;
	imageSampleDesc.MipLevels = imageSampleDesc.ArraySize = 1;
	imageSampleDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	imageSampleDesc.SampleDesc.Count = 1;
	imageSampleDesc.Usage = D3D11_USAGE_DEFAULT;
	imageSampleDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	imageSampleDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	imageSampleDesc.MiscFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	HRESULT createTextureSucc = devicePtr->CreateTexture2D(&imageSampleDesc, NULL, &pTexture); //Create an empty texture resource
	//HRESULT deviceContext->UpdateSubresource();
	assert(SUCCEEDED(createTextureSucc));

	deviceContext->UpdateSubresource(pTexture, 0, NULL, subResData.pSysMem, 0, 0); //Send that magical stuff to the resource

	//deviceContext->GenerateMips(textureView);
	deviceContext->PSSetShaderResources(0, 1, &textureView); //Add texture to shader

	D3D11_SAMPLER_DESC textureSamplerDesc; // Might need to fill with data
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
	HRESULT samplerStateSucc = devicePtr->CreateSamplerState(&textureSamplerDesc, &sampleState);
	assert(SUCCEEDED(samplerStateSucc));

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	//--------------------------------------
	//TRIANGLE SETTINGS
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout((ID3D11InputLayout*)input_layout_ptr);
	deviceContext->VSSetShader((ID3D11VertexShader*)vertexPtr, NULL, 0);
	deviceContext->PSSetShader((ID3D11PixelShader*)pixelPtr, NULL, 0);
	UINT stride = nrOfFloatsPerVertex * sizeof(float);
	UINT offset = 0;
	
	//To handle incoming 'messages, signals', this is for polling.
	//GetMessage() is a problem as it blocks when no message is available. (Busy wait). GetMessage(&msg, NULL, 0, 0)
	//Use PeekMessage() instead.
	
	//clock_t clock;
	//time_t currTime;
	//currTime = time(0);

	std::chrono::high_resolution_clock time;
	std::chrono::high_resolution_clock prev;
	auto start = time.now();

	while (TRUE)
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
		auto sampleTime = time.now();

		std::chrono::duration<float> elapsed_seconds = sampleTime - start;

		rotation += 0.005;
		cBuffer.rotationMatrix = DirectX::XMMATRIX
		(
			cos(rotation), 0, sin(rotation),		0,
			0,				1,			0,			0,
			-sin(rotation), 0, cos(rotation),		0,
			0,				0,			0,			1
		);

		deviceContext->UpdateSubresource(constantBuffer, 0, NULL, &cBuffer, 0, 0);

		//END CONSTANT BUFFER UPDATE-------------------------------------------------


		RECT viewportRect;
		GetClientRect(hWnd, &viewportRect);
		D3D11_VIEWPORT port = { 
			0.f,
			0.f,
			(float)(viewportRect.right - viewportRect.left),
			(float)(viewportRect.bottom - viewportRect.top),0.f,1.f
		};
		deviceContext->RSSetViewports(1, &port);
		float background_color[4] = {0.5f, 0.f, 0.f, 0.f };
		deviceContext->ClearRenderTargetView(renderTargetPtr, background_color);
		deviceContext->OMSetRenderTargets(1, &renderTargetPtr, NULL);

		//MAGICAL TRIANGLE-------------------------

		deviceContext->IASetVertexBuffers(0, 1, &vertexBufferPtr,
			&stride, &offset); //Stride, offset
		deviceContext->Draw(nrOfVertices, 0);

		swapChainPtr->Present(1, 0);
	}

	return (int)msg.wParam;
}