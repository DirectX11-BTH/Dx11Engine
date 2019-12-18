#pragma once
#define WIN32_LEAN_AND_MEAN
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <assert.h>
#include <cassert>
#include <iostream>
#include <Windows.h>
//#include <d3dcompiler.h>
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
#include "Mesh.h"
#include "EngineObject.h"

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")

const float FLOATS_PER_VERTEX = 12.f;

namespace wrl = Microsoft::WRL;

struct VS_CONSTANT_MATRIX_BUFFER
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX cameraMatrix;
};

struct PS_CONSTANT_LIGHT_BUFFER
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX cameraMatrix;
};

class DxHandler
{
private:
		
public:
	static  ID3D11Device* devicePtr;
	static  ID3D11DeviceContext* contextPtr;
	static  HINSTANCE hInstance;

	std::vector<ID3D11Buffer*> loadedBuffers;

	DxHandler(HWND& hWnd)
	{
		configureSwapChain(hWnd);
		initalizeDeviceContextAndSwapChain();
	}

	static IDXGISwapChain* swapChainPtr;
	static ID3D11RenderTargetView* renderTargetPtr;
	static DXGI_SWAP_CHAIN_DESC swapDesc; //= DXGI_SWAP_CHAIN_DESC{ 0 };
	//static ID3D11Buffer* vertexShaderBuffer;

	static ID3DBlob* vertexShaderBuffer;
	static ID3DBlob* pixelShaderBuffer;
	static ID3D11PixelShader* pixelPtr;
	static ID3D11VertexShader* vertexPtr;
	static ID3D11InputLayout* input_layout_ptr;

	template <typename T>
	ID3D11Buffer*& createVSConstBuffer(T cStruct);
	template <typename T>
	ID3D11Buffer*& createPSConstBuffer(T cStruct);

	void initalizeDeviceContextAndSwapChain();
	void configureSwapChain(HWND& hWnd);
	void setupInputLayout();
	ID3D11Buffer* createVertexBuffer(Mesh& mesh);

	void setCullingMode(D3D11_CULL_MODE mode);

	void setupPShader(const wchar_t fileName[]);
	void setupVShader(const wchar_t fileName[]);

	void draw(EngineObject& drawObject);
};


