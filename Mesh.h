#pragma once
#include <vector>
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
	ID3D11Buffer* vertexBuffer = NULL;
	int nrOfVertices = 0;
	std::vector<Vertex> vertices;
};