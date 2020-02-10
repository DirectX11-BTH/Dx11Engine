#pragma once
#include "DXHandler.h"
#include <string>
#include <stdint.h>


//./heightmap.png

class TerrainGenerator
{
	
	
public:
	Mesh& generateFromHeightMap(std::string textureName);
	void readTexture(std::string textureName);

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };

};