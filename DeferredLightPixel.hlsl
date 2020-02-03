/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	February 1st 2020

	Last pass, goes over the scene & calculates light and colors objects.
	But how does it get information about the object?
	Such as specular per mesh?
*/

cbuffer PS_CONSTANT_BUFFER
{
	float4 lightPos;
	float4 ambientMeshColor;
	float4 diffueMeshColor;
	float4 specularMeshColor;
	float4 camPos;
	row_major float4x4 worldViewProjectionMatrix;
	float4 specularExponent; //Only use x value
}

Texture2D color : register(t0); //texture that holds the color information
Texture2D normals : register(t1); //texture that holds the normals

SamplerState mySampler : register(s0); //Since we need to sample points from both & combine into one output

struct INPUT
{
	float4 pos : SV_POSITION;
	float2 textureCoord : TEXCOORD0; //Pixel coordinate fed to the shader
};

float4 main(INPUT input) : SV_TARGET
{
	//Per pixel attributes
	float4 sampledColor = color.Sample(mySampler, input.textureCoord);
	float4 sampledNormal = normals.Sample(mySampler, input.textureCoord);

	return sampledColor; //No light just for testing.
}