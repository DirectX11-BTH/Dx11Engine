#pragma once
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>
#include <math.h>
#include <vector>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <assert.h>
#include <cmath>
#include "DXHandler.h"
class SsaoClass
{
private:
	static float lerp(float a, float b, float t);

public:
	static const int nrOfVecs = 32;
	static const int noiseSize = 4;

	static ID3D11Texture2D* randomVecTexture;
	static ID3D11Texture2D* randomVecNoise;

	static ID3D11PixelShader* SSAOPixelPtr;
	static ID3D11VertexShader* SSAOVertexPtr;
	static ID3DBlob* SSAOVertexShaderBuffer;
	static ID3DBlob* SSAOPixelShaderBuffer;

	static ID3D11Texture2D* randomVecRenderTargetTexture;
	static ID3D11RenderTargetView* randomVecRenderTargetView;
	static ID3D11ShaderResourceView* randomVecShaderResourceView;

	static ID3D11Texture2D* randomNoiseRenderTargetTexture;
	static ID3D11RenderTargetView* randomNoiseRenderTargetView;
	static ID3D11ShaderResourceView* randomNoiseShaderResourceView;

	static ID3D11Texture2D* SSAOBuffRenderTargetTexture;
	static ID3D11RenderTargetView* SSAOBuffRenderTargetView;
	static ID3D11ShaderResourceView* SSAOBuffShaderResourceView;

	static void setupShaders();

	static void generateOcclusionBuffer();

	//static int nrOfVecs; //Can not go past 32
	static DirectX::XMVECTOR randomVectors[nrOfVecs*nrOfVecs];
	static DirectX::XMVECTOR noiseVectors[noiseSize*noiseSize];
	static void generateRandomVectors();
	static void generateNoiseTexture();
};