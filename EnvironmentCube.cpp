#include "EnvironmentCube.h"
using namespace DirectX;

void EnvironmentCube::buildCubeMap()
{
	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	D3D11_RENDER_TARGET_VIEW_DESC  renderTargetDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;



	//TEXTURE DESC
	texDesc.Width = 720;
	texDesc.Height = 720;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };

	HRESULT texSucc = DxHandler::devicePtr->CreateTexture2D(&texDesc, NULL, &renderTargetTexture);
	assert(SUCCEEDED(texSucc));

	//RENDERTARGET DESC
	renderTargetDesc.Format = texDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY; //Means it'll be handled as an array of textures
	renderTargetDesc.Texture2DArray.ArraySize = 1;
	renderTargetDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		renderTargetDesc.Texture2DArray.FirstArraySlice = i;

		HRESULT targetSucc = DxHandler::devicePtr->CreateRenderTargetView(renderTargetTexture, &renderTargetDesc, &renderTargetView[i]);
		assert(SUCCEEDED(targetSucc));
	}

	////
	shaderResourceDesc.Format = texDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceDesc.Texture2DArray.MostDetailedMip = 0;
	shaderResourceDesc.Texture2DArray.MipLevels = -1;
	HRESULT shaderResourceSucc = DxHandler::devicePtr->CreateShaderResourceView(renderTargetTexture, &shaderResourceDesc, &shaderResourceView);
	assert(SUCCEEDED(shaderResourceSucc));

	D3D11_TEXTURE2D_DESC depthDesc{ 0 };
	depthDesc.Width = texDesc.Width;
	depthDesc.Height = texDesc.Height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;


	DxHandler::devicePtr->CreateTexture2D(&depthDesc, NULL, &depthBuffer);
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	depthStencilViewDesc.Format = depthDesc.Format;
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	HRESULT depthStencilViewSucc = DxHandler::devicePtr->CreateDepthStencilView(depthBuffer, &depthStencilViewDesc, &depthStencilView);

	DxHandler::devicePtr->CreateTexture2D(&depthDesc, NULL, &depthBuffer);
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	HRESULT depthStencilStateSucc = DxHandler::devicePtr->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	depthBuffer->Release(); 

	port = {0, 0, (float)texDesc.Width, (float)texDesc.Height, 0, 1.f};
}

void EnvironmentCube::buildCameras(float x, float y, float z)
{
	XMVECTOR targets[6] =
	{
		XMVectorSet(x + 1, y, z, 1), //right x
		XMVectorSet(x - 1, y, z, 1), //left x
		XMVectorSet(x, y + 1, z, 1), //up
		XMVectorSet(x, y - 1, z, 1), //down
		XMVectorSet(x, y, z + 1, 1), //front
		XMVectorSet(x, y, z - 1, 1) //back
	};

	XMVECTOR upVectors[6] =
	{
		XMVectorSet(0, 1, 0, 0),
		XMVectorSet(0, 1, 0, 0),
		XMVectorSet(0, 0, -1, 0),
		XMVectorSet(0, 0, 1, 0),
		XMVectorSet(0, 1, 0, 0),
		XMVectorSet(0, 1, 0, 0),
	};

	XMVECTOR position = XMVectorSet(x, y, z, 1);

	for (int i = 0; i < 6; i++)
	{
		faceCameras[i] = cubeCamera();
		faceCameras[i].cameraPosition = position;
		faceCameras[i].cameraView = DirectX::XMMatrixLookAtLH(position, targets[i], upVectors[i]);
		faceCameras[i].cameraProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH( //Creates projection space
			3.14f / 2,					//FovAngleY, height angle of perspective in radians, should be 90 deg
			(float)720 / (float)720,	//AspectRatio, width/height of window
			0.1f,							//NearZ, how close we render
			10000.f							//FarZ how far we render
		);
	}
}

EnvironmentCube::EnvironmentCube()
{
	object.meshes.push_back(ObjParser::readFromObj("./TestModel/actualCube.obj")); //Fix this
}

void EnvironmentCube::render()
{
}
