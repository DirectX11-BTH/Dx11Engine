#include "DeferredRenderBuffer.h"

DeferredRenderBuffer::~DeferredRenderBuffer()
{
	renderTargetTexture->Release();
	renderTargetView->Release();

	shaderResourceView->Release();

}

DeferredRenderBuffer::DeferredRenderBuffer()
{
}

DeferredRenderBuffer::DeferredRenderBuffer(int textureWidth, int textureHeight)
{
	//init(textureWidth, textureHeight);
}

void DeferredRenderBuffer::init(int width, int height)
{
	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;


	texWidth = width;
	texHeight = height;
	//TEXTURE DESC
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //RGBA 4 lyf
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	
	HRESULT texSucc = DxHandler::devicePtr->CreateTexture2D(&texDesc, NULL, &renderTargetTexture);
	assert(SUCCEEDED(texSucc));
	//RENDERTARGET DESC
	renderTargetDesc.Format = texDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; //Means it'll be handled as a 2D array
	renderTargetDesc.Texture2D.MipSlice = 0;
	HRESULT targetViewSucc = DxHandler::devicePtr->CreateRenderTargetView(renderTargetTexture, &renderTargetDesc, &renderTargetView);
	assert(SUCCEEDED(targetViewSucc));

	////
	shaderResourceDesc.Format = texDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	HRESULT shaderResourceSucc = DxHandler::devicePtr->CreateShaderResourceView(renderTargetTexture, &shaderResourceDesc, &shaderResourceView);
	assert(SUCCEEDED(shaderResourceSucc));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	HRESULT succ = DxHandler::devicePtr->CreateUnorderedAccessView(renderTargetTexture, &uavDesc, &unorderedAccessView);
	assert(SUCCEEDED(succ));
}
