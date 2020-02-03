/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	February 1st 2020

	Deferred Pixel Shader implementation
*/


Texture2D myTexture : register(t0);
SamplerState mySampler;

struct PS_INPUT
{
	float4 pixelNormal : NORMAL;
	float4 pixelPosition : SV_POSITION;
	float4 texCoord : TEXCOORD0;
};

struct PS_DEFERRED_OUTPUT
{
	float4 pixelColor : SV_Target0; //This is one buffer, that will hold colors
	float4 pixelNormal : SV_Target1; //Another buffer that holds the normals
};

PS_DEFERRED_OUTPUT main(PS_INPUT input) : SV_Target
{
	PS_DEFERRED_OUTPUT output;

	output.pixelNormal = input.pixelNormal;
	output.pixelColor = myTexture.Sample(mySampler, input.texCoord.xy);
	
	return output;
}