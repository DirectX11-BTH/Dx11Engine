#pragma once
#include "DXHandler.h"
#include <string>
#include <stdint.h>


//./heightmap.png

std::vector<Vertex> generateQuad(DirectX::XMFLOAT3 corner0, DirectX::XMFLOAT3 corner1, DirectX::XMFLOAT3 corner2, DirectX::XMFLOAT3 corner3);

class WaterPlane
{
public:
	void generateWaterPlane();
	void readTexture(std::string textureName);

	static Mesh waterGeometry;
	static float scaling;
	float quadsPerAxis = 30;

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SAMPLER_DESC textureSamplerDesc;
	D3D11_TEXTURE2D_DESC  imageSampleDesc = { 0 };
	std::vector<Vertex> generateQuad(DirectX::XMFLOAT3 corner0, DirectX::XMFLOAT3 corner1, DirectX::XMFLOAT3 corner2, DirectX::XMFLOAT3 corner3);
};