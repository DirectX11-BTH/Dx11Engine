/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/


Texture2D mytexture;
SamplerState mysampler;

cbuffer PS_CONSTANT_BUFFER
{
	float4 light;
}

struct VS_OUTPUT
{
	float4 vColour : COLOR;
	float4 vPosition : SV_POSITION;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 vInterpolatedPosition : POSITION;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 textureColor = mytexture.Sample(mysampler, input.vUV);
	//float3 surfaceToLightV = normalize(light.xyz - input.vInterpolatedPosition);
	//float diffuseStrength = clamp(dot(input.vNormal, surfaceToLightV), 0, 1);
	//float ambientStrength = 0.1f;

	//float4 camPos = float4(0, 0, 0, 0); //Since no camera matrix, specular
	//float4 lookVector = normalize(camPos - input.vInterpolatedPosition); //Specular
	//float4 reflectionVec = normalize(reflect(float4(surfaceToLightV,0), input.vNormal)); //Specular
	//float specStrength = pow(clamp(dot(reflectionVec, lookVector), 0, 1), 100);
	
	return input.vColour;
	//return textureColor;

	//return input.vColour;//diffuseStrength+ambientStrength)*textureColor; //Should be black when correct cause rgb -1?

	//return float4(input.vNormal, 0);
}