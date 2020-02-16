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

ID3D11Texture2D* SsaoClass::SSAOBuffRenderTargetTexture = nullptr;
ID3D11RenderTargetView* SsaoClass::SSAOBuffRenderTargetView = nullptr;
ID3D11ShaderResourceView* SsaoClass::SSAOBuffShaderResourceView = nullptr;

ID3D11PixelShader* SsaoClass::SSAOPixelPtr;
ID3D11VertexShader* SsaoClass::SSAOVertexPtr;
ID3DBlob* SsaoClass::SSAOVertexShaderBuffer;
ID3DBlob* SsaoClass::SSAOPixelShaderBuffer;

float SsaoClass::lerp(float a, float b, float t)
{
	return (a * (1.0f - t)) + (b * t);
}

void SsaoClass::setupShaders()
{
	//Pixel
	ID3DBlob* errorMessage;

	HRESULT SSAOPixelShaderSucc = D3DCompileFromFile(
		L"SSAOPixel.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0", //Pixel shader
		0,
		0,
		&SSAOPixelShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(SSAOPixelShaderSucc));

	HRESULT createSSAOPixelShaderSucc = DxHandler::devicePtr->CreatePixelShader(SSAOPixelShaderBuffer->GetBufferPointer(), SSAOPixelShaderBuffer->GetBufferSize(), NULL, &SSAOPixelPtr);
	assert(SUCCEEDED(createSSAOPixelShaderSucc));
	//=============================================
	//Vertex

	HRESULT	SSAOVertexShaderSucc = D3DCompileFromFile(
		L"SSAOVertex.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0", //Vertex shader
		0,
		0,
		&SSAOVertexShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(SSAOVertexShaderSucc));

	HRESULT createSSAOVertexShaderSucc = DxHandler::devicePtr->CreateVertexShader(SSAOVertexShaderBuffer->GetBufferPointer(), SSAOVertexShaderBuffer->GetBufferSize(), NULL, &SSAOVertexPtr);
	assert(SUCCEEDED(createSSAOVertexShaderSucc));
}

void SsaoClass::generateOcclusionBuffer()
{
	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;


	int texWidth = DxHandler::WIDTH;
	int texHeight = DxHandler::HEIGHT;
	//TEXTURE DESC
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //RGBA 4 lyf
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	HRESULT texSucc = DxHandler::devicePtr->CreateTexture2D(&texDesc, NULL, &SSAOBuffRenderTargetTexture);
	assert(SUCCEEDED(texSucc));
	//RENDERTARGET DESC
	renderTargetDesc.Format = texDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; //Means it'll be handled as a 2D array
	renderTargetDesc.Texture2D.MipSlice = 0;
	HRESULT targetViewSucc = DxHandler::devicePtr->CreateRenderTargetView(SSAOBuffRenderTargetTexture, &renderTargetDesc, &SSAOBuffRenderTargetView);
	assert(SUCCEEDED(targetViewSucc));

	////
	shaderResourceDesc.Format = texDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	HRESULT shaderResourceSucc = DxHandler::devicePtr->CreateShaderResourceView(SSAOBuffRenderTargetTexture, &shaderResourceDesc, &SSAOBuffShaderResourceView);
	assert(SUCCEEDED(shaderResourceSucc));
}

void SsaoClass::generateRandomVectors()
{
	//generate a random set of points to sample from
	srand (time(NULL));
	for (int i = 0; i < nrOfVecs*nrOfVecs; i++)
	{
		//generate half dome of random vectors
		float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2))-1; // -1 - 1 x
		float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2))-1; // -1 - 1 y
		float randZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1)); // 0-1 z
		DirectX::XMVECTOR randomVec = XMVectorSet(randX, randY, randZ, 0);
		
		randomVectors[i] = randomVec;
		randomVec = XMVector3Normalize(randomVectors[i]);

		float vecScale = (float)i / (float)nrOfVecs;
		vecScale = lerp(0.1, 1, vecScale*vecScale); //Make them bunch up closer to the origin point
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
	// generate the noise, or the tangent in this case randomly that will help us rotate the sample points randomly
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