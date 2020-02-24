#pragma once
#include "EngineObject.h"
#include "ObjParser.h"
#include "DXHandler.h"

struct cubeCamera
{
	DirectX::XMVECTOR cameraPosition; 
	DirectX::XMVECTOR cameraTarget;
	DirectX::XMVECTOR cameraUp;
	DirectX::XMMATRIX cameraProjectionMatrix; //How far it can see and what it can see

	DirectX::XMMATRIX cameraView;//Tells you about orientation and positioning, essentially what it can see
};

class EnvironmentCube
{
private:
public:
	void buildCubeMap();
	void buildCameras(float x, float y, float z);

	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView[6]; //6 sides of a cube
	ID3D11ShaderResourceView* shaderResourceView;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthBuffer;

	cubeCamera faceCameras[6];
	D3D11_VIEWPORT port;

	EngineObject object;
	EnvironmentCube();
	void render();
};