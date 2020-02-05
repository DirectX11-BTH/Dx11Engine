#include "Mesh.h"

Mesh::~Mesh()
{
	if (vertexBuffer)
	{
		vertexBuffer->Release();
	}

	if (indexBuffer)
	{
		//indexBuffer->Release();
	}
}
