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
#include "Camera.h"

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")

const float FLOATS_PER_VERTEX = 12.f;

namespace wrl = Microsoft::WRL;

const int PER_OBJECT_CBUFFER_SLOT = 0;
const int CAMERA_CBUFFER_SLOT = 1;

struct VS_CONSTANT_MATRIX_BUFFER
{
	DirectX::XMMATRIX worldViewProjectionMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct VS_CONSTANT_CAMERA_BUFFER
{
	DirectX::XMMATRIX worldViewProjectionMatrix;
	DirectX::XMMATRIX cameraView;

	DirectX::XMVECTOR cameraPosition; //= DirectX::XMVectorSet(0.0f, 0.0f, -8.f, 0.0f); //0.5f to shoot the camera 'back' a bit
	DirectX::XMVECTOR cameraTarget; //= DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR cameraUp; //= DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};

struct PS_CONSTANT_LIGHT_BUFFER
{
	DirectX::XMVECTOR lightPos;
	Color ambientMeshColor;
	Color diffueMeshColor;
	Color specularMeshColor;

	DirectX::XMVECTOR camPos;
	DirectX::XMMATRIX worldViewProjectionMatrix;

	DirectX::XMVECTOR specularExponent;
};

class DxHandler
{
private:
		
public:
	static  ID3D11Device* devicePtr;
	static  ID3D11DeviceContext* contextPtr;
	static  HINSTANCE hInstance;

	std::vector<ID3D11Buffer*> loadedVSBuffers;
	std::vector<ID3D11Buffer*> loadedPSBuffers;

	DxHandler(HWND& hWnd)
	{
		configureSwapChain(hWnd);
		initalizeDeviceContextAndSwapChain();
	}

	~DxHandler();
	static IDXGISwapChain* swapChainPtr;
	static ID3D11RenderTargetView* renderTargetPtr;
	static DXGI_SWAP_CHAIN_DESC swapDesc; //= DXGI_SWAP_CHAIN_DESC{ 0 };
	//static ID3D11Buffer* vertexShaderBuffer;

	static ID3DBlob* vertexShaderBuffer;
	static ID3DBlob* pixelShaderBuffer;
	static ID3D11PixelShader* pixelPtr;
	static ID3D11VertexShader* vertexPtr;
	static ID3D11InputLayout* input_layout_ptr;

	static ID3D11DepthStencilView* depthStencil;
	static ID3D11Texture2D* depthBuffer;

	static ID3D11Buffer* PSConstBuff;

	ID3D11Buffer* createVSConstBuffer(VS_CONSTANT_MATRIX_BUFFER& matrix);
	ID3D11Buffer* createVSConstBuffer(VS_CONSTANT_CAMERA_BUFFER& matrix);

	ID3D11Buffer*& createPSConstBuffer(PS_CONSTANT_LIGHT_BUFFER& matrix);

	void initalizeDeviceContextAndSwapChain();
	void configureSwapChain(HWND& hWnd);
	void setupInputLayout();

	void setupLightBuffer();

	ID3D11Buffer* createVertexBuffer(Mesh& mesh);
	ID3D11Buffer* createIndexBuffer(Mesh& mesh);

	void setCullingMode(D3D11_CULL_MODE mode);

	void setupPShader(const wchar_t fileName[]);
	void setupVShader(const wchar_t fileName[]);

	void setupDepthBuffer(int widthOfRenderWindow, int heightOfRenderWindow);

	void draw(EngineObject& drawObject, bool perspective = true, bool firstPass = true);
	void drawIndexedMesh(EngineObject& drawObject);
};

