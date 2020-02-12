#pragma once
#include "DXHandler.h"
#include <string>
#include <stdint.h>


//./heightmap.png

std::vector<Vertex> generateQuad(DirectX::XMFLOAT3 corner0, DirectX::XMFLOAT3 corner1, DirectX::XMFLOAT3 corner2, DirectX::XMFLOAT3 corner3);

class TerrainGenerator
{
	
	
public:
	void generateFromHeightMap(std::string textureName);
	void readTexture(std::string textureName);

	static Mesh heightTerrain;

	static float scaling;
	static float verticalScaling;
	static int width;
	static int height;

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };

};