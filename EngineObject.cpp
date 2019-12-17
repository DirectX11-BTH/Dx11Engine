#include "EngineObject.h"
#include "DXHandler.h"

Mesh EngineObject::readMesh(float mesh[], int nrOfVertices)
{
	int vertexStride = FLOATS_PER_VERTEX; //12 floats per vertex in array XYZ, RGB, UV, NX,NY,NZ

	Mesh createdMesh;
	for (int i = 0; i < nrOfVertices; i++)
	{
		float x = mesh[0 + vertexStride * i];
		float y = mesh[1 + vertexStride * i];
		float z = mesh[2 + vertexStride * i];

		float r = mesh[3 + vertexStride * i];
		float g = mesh[4 + vertexStride * i];
		float b = mesh[5 + vertexStride * i];
		float a = mesh[6 + vertexStride * i];

		float u = mesh[7 + vertexStride * i];
		float v = mesh[8 + vertexStride * i];

		float nx = mesh[9 + vertexStride * i];
		float ny = mesh[10 + vertexStride * i];
		float nz = mesh[11 + vertexStride * i];

		createdMesh.vertices.push_back(Vertex{x,y,z,r,g,b,a,u,v,nx,ny,nz});
	}

	createdMesh.nrOfVertices = nrOfVertices;

	this->meshes.push_back(createdMesh);

	return createdMesh;
}

void EngineObject::readTextureFromFile(const wchar_t* textureName) //Kanske bör ändras till path?
{
	HRESULT readTextureResult = DirectX::CreateWICTextureFromFile(DxHandler::devicePtr, textureName, &texture, &textureView, 0);

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


}