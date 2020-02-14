#include "SsaoClass.h"
using namespace DirectX;

DirectX::XMVECTOR SsaoClass::randomVectors[nrOfVecs*nrOfVecs];
DirectX::XMVECTOR SsaoClass::noiseVectors[noiseSize*noiseSize];

ID3D11Texture2D* SsaoClass::randomVecTexture = nullptr;
ID3D11Texture2D* SsaoClass::randomVecNoise = nullptr;

ID3D11Texture2D* SsaoClass::randomVecRenderTargetTexture = nullptr;
ID3D11RenderTargetView* SsaoClass::randomVecRenderTargetView = nullptr;
ID3D11ShaderResourceView* SsaoClass::randomVecShaderResourceView = nullptr;

ID3D11Texture2D* SsaoClass::randomNoiseRenderTargetTexture = nullptr;
ID3D11RenderTargetView* SsaoClass::randomNoiseRenderTargetView = nullptr;
ID3D11ShaderResourceView* SsaoClass::randomNoiseShaderResourceView = nullptr;

float SsaoClass::lerp(float a, float b, float t)
{
	return (a * (1.0f - t)) + (b * t);
}

void SsaoClass::generateRandomVectors()
{
	//generate a random set of points to sample from
	srand (time(NULL));
	for (int i = 0; i < nrOfVecs*nrOfVecs; i++)
	{
		float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2))-1;
		float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2))-1;
		float randZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));
		DirectX::XMVECTOR randomVec = XMVectorSet(randX, randY, randZ, 0);
		
		randomVectors[i] = randomVec;
		randomVec = XMVector3Normalize(randomVectors[i]);

		float vecScale = (float)i / (float)nrOfVecs;
		vecScale = lerp(0.1, 1, vecScale*vecScale);
		randomVectors[i] *= vecScale;
	}

	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;


	int texWidth = nrOfVecs;
	int texHeight = nrOfVecs;
	//TEXTURE DESC
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //RGBA 4 lyf
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initialData; //To hold the vectors
	initialData.pSysMem = randomVectors;
	initialData.SysMemPitch = sizeof(XMVECTOR);

	//Now initialises with the vector data
	HRESULT texSucc = DxHandler::devicePtr->CreateTexture2D(&texDesc, &initialData, &randomVecRenderTargetTexture);
	assert(SUCCEEDED(texSucc));

	////
	shaderResourceDesc.Format = texDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	HRESULT shaderResourceSucc = DxHandler::devicePtr->CreateShaderResourceView(randomVecRenderTargetTexture, &shaderResourceDesc, &randomVecShaderResourceView);
	assert(SUCCEEDED(shaderResourceSucc));

	
}

void SsaoClass::generateNoiseTexture()
{
	for (int i = 0; i < noiseSize*noiseSize; i++)
	{
		float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;
		float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;

		XMVECTOR randomNoiseVector = XMVectorSet(randX, randY, 0, 0);
		randomNoiseVector = XMVector3Normalize(randomNoiseVector);
		noiseVectors[i] = randomNoiseVector;
		
	}


	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;

	int texWidth = noiseSize;
	int texHeight = noiseSize;

	//TEXTURE DESC
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //RGBA 4 lyf
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initialData; //To hold the vectors
	initialData.pSysMem = noiseVectors;
	initialData.SysMemPitch = sizeof(XMVECTOR);
	
	HRESULT texSucc = DxHandler::devicePtr->CreateTexture2D(&texDesc, &initialData, &randomNoiseRenderTargetTexture);
	assert(SUCCEEDED(texSucc));

	shaderResourceDesc.Format = texDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	HRESULT shaderResourceSucc = DxHandler::devicePtr->CreateShaderResourceView(randomNoiseRenderTargetTexture, &shaderResourceDesc, &randomNoiseShaderResourceView);
	assert(SUCCEEDED(shaderResourceSucc));
}