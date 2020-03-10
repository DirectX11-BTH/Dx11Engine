/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/


Texture2D mytexture : register(t0);
Texture2D NormalMapTexture : register(t1);
TextureCube EnvironmentTexture : register(t2);
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

	row_major float4x4 worldMatrix;
	row_major float4x4 viewMatrix;
	row_major float4x4 projMatrix;
	row_major float4x4 viewInverseMatrix;
	row_major float4x4 worldInverseMatrix;
	float4 uvDisplacement;

	bool hasNormalMap;
	bool hasTexture;
	bool environmentMap;
	bool glowingObject;
	bool isWater;
	//If you add something that's not a bool after here you will need padding.
}

struct VS_OUTPUT //Output from geometry shader
{
	float4 vPosition : SV_POSITION;	
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

struct PS_OUTPUT //Output to gbuffers
{
	float4 vPosition : SV_TARGET0;
	float4 vColour : SV_TARGET1;
	float4 vNormal : SV_TARGET2;
	float4 vGlow : SV_TARGET3;
};

PS_OUTPUT main(VS_OUTPUT input) : SV_Target
{

	PS_OUTPUT output;
	output.vPosition = input.positionInWorldSpace;

	if (isWater)
	{
		input.vUV = input.vUV + uvDisplacement;
	}

	if (hasTexture == true) 
	{
		output.vColour = mytexture.Sample(mysampler, input.vUV.xy);
	}
	else
	{
		output.vColour = float4(0.8, 0.8, 0.8, 1);
	}
		
	if (hasNormalMap)
	{
		float3 loadedNormal = NormalMapTexture.Sample(mysampler, input.vUV);
		float3 tangent = normalize(input.vTangent - dot(input.vTangent, input.vNormal) * input.vNormal);
		float3 bitangent = normalize(cross(loadedNormal, tangent));

		float3x3 tbn = float3x3(tangent, bitangent, loadedNormal); //This will move things into 'texture space' or 'tangent space' in order to support rotations of the object without distortion.

		input.vNormal = normalize(float4(mul(loadedNormal, tbn), 0));
	}

	if (environmentMap)
	{		
		float3 camToPixelVec = (input.positionInWorldSpace.xyz - camPos.xyz);
		float3 camToPixelReflected = normalize(reflect(camToPixelVec, input.vNormal.xyz));

		output.vColour = EnvironmentTexture.Sample(mysampler, camToPixelReflected);
		output.vColour.w = 2; //To avoid light calculations later. Is normally not above 1.
	}

	if (glowingObject)
	{
		output.vGlow = float4(1, 0, 0, 1); //White glow.
		output.vColour.w = 2; //To avoid light calculations later. Is normally not above 1.
	}
	else
		output.vGlow = float4(0, 0, 0, 0);
		
	output.vNormal = input.vNormal;
	return output;

}