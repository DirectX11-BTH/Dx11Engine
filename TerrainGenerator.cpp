#include "TerrainGenerator.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "./PngLoader/stb_image.h"

Mesh& TerrainGenerator::generateFromHeightMap(std::string textureName)
{
	Mesh terrain;
	readTexture(textureName);

	//Read texture

	//Generate flat plane

	//Magic
	//??
	//profit

	return terrain;
}
void TerrainGenerator::readTexture(std::string textureName)
{
	int width, height, bpp;
	width = 20;
	height = 20;
	bpp = 4; //RGBA, bits per pixel

    uint8_t* rgb_image = stbi_load(textureName.data(), &width, &height, &bpp, 3);

	float r = (float)rgb_image[0+0];
	float g = (float)rgb_image[0+1];
	float b = (float)rgb_image[0+2];
	float a = (float)rgb_image[0+3];

    stbi_image_free(rgb_image);
}