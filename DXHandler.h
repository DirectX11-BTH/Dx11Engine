#pragma once
#define WIN32_LEAN_AND_MEAN
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <assert.h>
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

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;

struct graphicsTuple
{

};

class DxHandler
{
private:
		
public:
	static  ID3D11Device* devicePtr;
	static  ID3D11DeviceContext* contextPtr;
	static  HINSTANCE hInstance;

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

	void initalizeDeviceContextAndSwapChain();
	void configureSwapChain(HWND& hWnd);
	void setupInputLayout();
	ID3D11Buffer* createVertexBuffer(Mesh& mesh);

	void setCullingMode(D3D11_CULL_MODE mode);

	void setupPShader(const wchar_t fileName[]);
	void setupVShader(const wchar_t fileName[]);

	void draw(Mesh& drawObject);
};


