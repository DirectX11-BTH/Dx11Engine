#pragma once
#include <vector>
#include <DirectXHelpers.h>
#include <SimpleMath.h>
class ID3D11Buffer;


struct Vertex
{
	float x, y, z = 0;
	float r, g, b, a = 1; //Default to white for debug
	float u, v = 0;
	float nx, ny, nz = 0;
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

	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;
	std::vector<DWORD> indicies;

	int nrOfVertices = 0;
	//int nrOfIndicies = 0;
	std::vector<Vertex> vertices;

	~Mesh();
};