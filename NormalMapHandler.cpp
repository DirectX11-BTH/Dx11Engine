#include "NormalMapHandler.h"
#include "DXHandler.h"


bool NormalMapHandler::loadNormalTextureFromFile(std::wstring textureName)
{
	HRESULT readTextureResult = DirectX::CreateWICTextureFromFile(DxHandler::devicePtr, textureName.data(), &texture, &textureView, 0);

	//Check if file could be loaded
	assert(SUCCEEDED(readTextureResult));

	imageSampleDesc.MipLevels = imageSampleDesc.ArraySize = 1;
	imageSampleDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	imageSampleDesc.SampleDesc.Count = 1;
	imageSampleDesc.Usage = D3D11_USAGE_DEFAULT;
	imageSampleDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	imageSampleDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	imageSampleDesc.MiscFlags = 0;

	//Reading image size from the desc
	ID3D11Texture2D *pTextureInterface = 0;
	ID3D11Resource *res;
	textureView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);
	imageSampleDesc.Width = static_cast<float>(desc.Width);
	imageSampleDesc.Height = static_cast<float>(desc.Height);

	//Releaseing res and interface, we need to release texture and textureView when killing object
	res->Release();
	pTextureInterface->Release();

	HRESULT createTextureResult = DxHandler::devicePtr->CreateTexture2D(&imageSampleDesc, NULL, &pTexture);
	assert(SUCCEEDED(createTextureResult));

	return SUCCEEDED(createTextureResult);
}

NormalMapHandler::NormalMapHandler()
{
}

NormalMapHandler::~NormalMapHandler()
{
	if (texture != nullptr)
		delete texture;
}
