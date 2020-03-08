#pragma once
#include <vector>
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <string>
class ID3D11Buffer;


struct Vertex
{
	float x, y, z = 0;
	float r, g, b, a = 1; //Default to white for debug
	float u, v = 0;
	float nx, ny, nz = 0;
	float tx, ty, tz = 0;
};

struct Color //For entire mesh color, needed for mtl parsing
{
	float r, g, b, a = 1; //Between 0-1.
};

class Mesh
{
public:
	//For going from local to world space. Starts with unit matrix.
	//DirectX::XMMATRIX worldSpaceMatrix = DirectX::XMMatrixIdentity();
	
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixIdentity();

	//Needed to parse MTL, assuming one material per mesh
	DirectX::XMFLOAT4 ambientMeshColor;
	DirectX::XMFLOAT4 diffuseMeshColor = DirectX::XMFLOAT4( 1, 1, 1, 1 );
	DirectX::XMFLOAT4 specularMeshColor;
	float shininess = 1.f;
	float specularExponent = 10.f;

	std::string textureName = "";

	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;
	std::vector<DWORD> indicies;

	int nrOfVertices = 0;
	//int nrOfIndicies = 0;
	std::vector<Vertex> vertices;

	~Mesh();
};