/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/


Texture2D mytexture;
SamplerState mysampler;

cbuffer PS_CONSTANT_BUFFER
{
	float4 lightPos;
	float4 ambientMeshColor;
	float4 diffuseMeshColor;
	float4 specularMeshColor;
	float4 camPos;
	row_major float4x4 worldViewProjectionMatrix;
	float4 specularExponent; //Only use x value
	float2 noiseScale;

	row_major float4x4 viewInverseMatrix;
	row_major float4x4 worldInverseMatrix;
}

struct VS_OUTPUT
{
	float4 vPosition : SV_POSITION;	
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	//float4 vInterpolatedPosition : POSITION;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

struct PS_OUTPUT
{
	float4 vPosition : SV_TARGET0;
	float4 vColour : SV_TARGET1;
	float4 vNormal : SV_TARGET2;
	float vTangent : SV_TARGET3;
	//float4 vInterpolatedPosition : POSITION;
};

PS_OUTPUT main(VS_OUTPUT input) : SV_Target
{
	PS_OUTPUT output;
	
	output.vPosition = input.positionInWorldSpace;//input.vPosition;

	/*if (hasTexture == true) //Always returns true, rip
	{
		//output.vColour = mytexture.Sample(mysampler, input.vUV);
		output.vColour = float4(1, 1, 1, 1);
	}
	else
	{
		output.vColour = diffuseMeshColor;
	}*/

	output.vColour = float4(1, 1, 1, 1);
		

	output.vNormal = input.vNormal;
	output.vTangent = input.vTangent;
	return output;

}