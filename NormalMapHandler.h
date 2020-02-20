#pragma once
#include <d3d11.h>
#include <string>
#include <WICTextureLoader.h>

class NormalMapHandler
{
private:


	ID3D11Resource* texture;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };


public:
	bool loadNormalTextureFromFile(std::wstring textureName);
	NormalMapHandler();
	~NormalMapHandler();

	ID3D11ShaderResourceView* textureView = nullptr;
};