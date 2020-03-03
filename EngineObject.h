#pragma once
#include "Mesh.h"
#include "string"
#include <d3d11.h>
#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")
#include <dxgi.h>
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1);
#include <WICTextureLoader.h>
#include "NormalMapHandler.h"

class EngineObject
{
private:
public:
	~EngineObject();

	bool hasTexture = false;
	bool hasNormalMap = false;
	bool glowingObject = false;

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView = nullptr;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };

	NormalMapHandler normalMapContainer;

	Mesh readMesh(float mesh[], int nrOfVertices);
	void readTextureFromFile(const wchar_t* textureName);
	std::vector<Mesh> meshes;
};