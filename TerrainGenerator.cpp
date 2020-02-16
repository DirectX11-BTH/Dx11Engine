#include "TerrainGenerator.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "./PngLoader/stb_image.h"

using namespace DirectX;

Mesh TerrainGenerator::heightTerrain;
float TerrainGenerator::scaling = 10.f;
float TerrainGenerator::verticalScaling = 150.f;


int TerrainGenerator::width = 0;
int TerrainGenerator::height = 0;

void TerrainGenerator::generateFromHeightMap(std::string textureName)
{
	//Read texture
	readTexture(textureName);

	//Generate flat plane
	//woops did it all in the readtexture function

	//Magic
	//??
	//profit
}
void TerrainGenerator::readTexture(std::string textureName)
{
	int bpp;
	width = 200;
	height = 200;
	bpp = sizeof(uint8_t)*4; //RGBA, bits per pixel

	heightTerrain.ambientMeshColor = DirectX::XMFLOAT4( 100/255,100/255,100/255, 100/255 );
	heightTerrain.diffuseMeshColor = DirectX::XMFLOAT4( 20/255,100/255,100/255, 100 / 255);
	DirectX::XMFLOAT4 specularMeshColor = DirectX::XMFLOAT4(100/255,100/255,100/255, 100 / 255);

    uint8_t* rgb_image = stbi_load(textureName.data(), &width, &height, &bpp, 1);

	//Row by row from top down

	float r = (float)rgb_image[0];
	float g = (float)rgb_image[1];
	float b = (float)rgb_image[2];
	float r1 = (float)rgb_image[3];
	float g1 = (float)rgb_image[4];
	float b1 = (float)rgb_image[5];

	float sizeMultiplier = scaling;

	for(int y = 0; y < height-1; y++)
	{
		for(int x = 0; x < width-1; x++)
		{
			XMFLOAT3 temp0, temp1, temp2, temp3; //bootiful
			temp0.x = x * sizeMultiplier;
			temp0.z = y * sizeMultiplier;
			temp0.y = (float)rgb_image[y*width + x + 0]/255.f;
			temp0.y *= verticalScaling;
					
			temp1.x = (x+1)* sizeMultiplier;
			temp1.z = (y) *sizeMultiplier;
			temp1.y = (float)rgb_image[y * width + x +1]/255.f;
			temp1.y *= verticalScaling;
			
			temp2.x = (x)*sizeMultiplier;
			temp2.z = (y+1) * sizeMultiplier;
			temp2.y = (float)rgb_image[(y+1)*width + x]/255.f;
			temp2.y *= verticalScaling;

			temp3.x = (x+1)* sizeMultiplier;
			temp3.z = (y+1) * sizeMultiplier;
			temp3.y = (float)rgb_image[(y + 1) * width + x + 1]/255.f;
			temp3.y *= verticalScaling;


			std::vector<Vertex> vertList = generateQuad(temp0,temp1,temp2,temp3);

			for (int i = 0; i < vertList.size(); i++)
			{
				heightTerrain.vertices.push_back(vertList.at(i));
			}
		}
	}

	DxHandler::createVertexBuffer(heightTerrain);
	//float a = (float)rgb_image[0+3];
	std::cout << "Nr of quads:" << (heightTerrain.vertices.size()/3)/2 << std::endl;;
    stbi_image_free(rgb_image);
}

std::vector<Vertex> generateQuad(XMFLOAT3 corner0, XMFLOAT3 corner1, XMFLOAT3 corner2, XMFLOAT3 corner3)
{
	//Generate default vertex w/ normal pointing up, default color grey defined in ambientMeshColor
	std::vector<Vertex> quadVertices;

	/*
   0 _ _ 1
	|  /|
	|/	|
   2     3
	*/

	Vertex vert0;
	vert0.x = corner0.x;
	vert0.y = corner0.y;
	vert0.z = corner0.z;

	vert0.r = 0.5;
	vert0.g = 0;
	vert0.b = 0;

	Vertex vert1;
	vert1.x = corner1.x;
	vert1.y = corner1.y;
	vert1.z = corner1.z;

	vert1.r = 0.7;
	vert1.g = 0;
	vert1.b = 0;
	
	Vertex vert2;
	vert2.x = corner2.x;
	vert2.y = corner2.y;
	vert2.z = corner2.z;

	vert2.r = 0;
	vert2.g = 0.5;
	vert2.b = 1;
	
	XMVECTOR corner0Vec = XMVectorSet(corner0.x, corner0.y, corner0.z, 0);
	XMVECTOR corner1Vec = XMVectorSet(corner1.x, corner1.y, corner1.z, 0);
	XMVECTOR corner2Vec = XMVectorSet(corner2.x, corner2.y, corner2.z, 0);
	XMVECTOR corner3Vec = XMVectorSet(corner3.x, corner3.y, corner3.z, 0);

	XMVECTOR c0Toc1 = corner1Vec - corner0Vec;
	XMVECTOR c0Toc2 = corner2Vec - corner0Vec;
	XMVECTOR normal1 = XMVector3Cross(c0Toc1, c0Toc2); //This is the first triangle's normal
	normal1 = XMVector3Normalize(-normal1);

	XMFLOAT4 normalFloatForm;
	XMStoreFloat4(&normalFloatForm, normal1);

	//this isnt pretty, i swear its not what it looks like hon
	vert0.nx = normalFloatForm.x;
	vert0.ny = normalFloatForm.y;
	vert0.nz = normalFloatForm.z;

	vert1.nx = normalFloatForm.x;
	vert1.ny = normalFloatForm.y;
	vert1.nz = normalFloatForm.z;

	vert2.nx = normalFloatForm.x;
	vert2.ny = normalFloatForm.y;
	vert2.nz = normalFloatForm.z;


	//----------------------------------------------------------------------------------------- vert 1 done
	Vertex vert3;
	vert3.x = corner1.x;
	vert3.y = corner1.y;
	vert3.z = corner1.z;

	vert3.r = 1;
	vert3.g = 1;
	vert3.b = 1;
	
	Vertex vert4;
	vert4.x = corner3.x;
	vert4.y = corner3.y;
	vert4.z = corner3.z;

	vert4.r = 0;
	vert4.g = 1;
	vert4.b = 1;

	Vertex vert5;
	vert5.x = corner2.x;
	vert5.y = corner2.y;
	vert5.z = corner2.z;

	vert5.r = 1;
	vert5.g = 1;
	vert5.b = 1;

	/*
   0 _ _ 1
	|  /|
	|/	|
   2     3
	*/

	//triangle 2 normals
	XMVECTOR c3Toc1 = corner1Vec - corner3Vec;
	XMVECTOR c3Toc2 = corner2Vec - corner3Vec;
	XMVECTOR normal2 = XMVector3Cross(c3Toc2, c3Toc1); //This is the first trian
	normal2 = XMVector3Normalize(-normal2);

	XMStoreFloat4(&normalFloatForm, normal2);

	//this isnt pretty, i swear its not what it looks like hon
	vert3.nx = normalFloatForm.x;
	vert3.ny = normalFloatForm.y;
	vert3.nz = normalFloatForm.z;

	vert4.nx = normalFloatForm.x;
	vert4.ny = normalFloatForm.y;
	vert4.nz = normalFloatForm.z;

	vert5.nx = normalFloatForm.x;
	vert5.ny = normalFloatForm.y;
	vert5.nz = normalFloatForm.z;
	
	quadVertices.push_back(vert0);
	quadVertices.push_back(vert1);
	quadVertices.push_back(vert2);
	quadVertices.push_back(vert3);
	quadVertices.push_back(vert4);
	quadVertices.push_back(vert5);

	return quadVertices;
}
