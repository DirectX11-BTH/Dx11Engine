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
	float2 noiseScale;
	row_major float4x4 worldMatrix;
	row_major float4x4 viewMatrix;
	row_major float4x4 projMatrix;
	row_major float4x4 viewInverseMatrix;
	row_major float4x4 worldInverseMatrix;
	bool hasNormalMap;
	bool hasTexture;
	bool environmentMap;

	bool isWater;
	float4 uvDisplacement;
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
	//float4 vInterpolatedPosition : POSITION;
};

PS_OUTPUT main(VS_OUTPUT input) : SV_Target
{
	PS_OUTPUT output;
	
	output.vPosition = input.positionInWorldSpace;//input.vPosition;

	if (isWater)
	{
		input.vUV = input.vUV + uvDisplacement;
		//output.vColour = float4(0, 1, 1, 1);
	}

	if (hasTexture == true) //Always returns true, rip
	{
		output.vColour = mytexture.Sample(mysampler, input.vUV);
		//output.vColour = float4(0, 1, 1, 1);
	}
	else
	{
		output.vColour = float4(1, 1, 1, 1);
	}

	//output.vColour = mytexture.Sample(mysampler, input.vUV);//float4(1, 1, 1, 1);
		
	if (hasNormalMap)
	{
		float3 loadedNormal = NormalMapTexture.Sample(mysampler, input.vUV);//NormalMapTexture.Load(float3(input.vPosition.xy, 0), 0).xyz;
		//float4 tangent = TangentTexture.Load(float3(input.vPosition.xy, 0), 0);
		float3 tangent = normalize(input.vTangent - dot(input.vTangent, input.vNormal) * input.vNormal);
		float3 bitangent = normalize(cross(loadedNormal, tangent));

		float3x3 tbn = float3x3(tangent, bitangent, loadedNormal); //This will move things into 'texture space' or 'tangent space' in order to support rotations of the object without distortion.

		input.vNormal = normalize(float4(mul(loadedNormal, tbn), 0));
	}

	if (environmentMap)
	{
		float3 pixelToCamVec =  (camPos.xyz - input.positionInWorldSpace.xyz);
		float3 pixelToCameraReflected = normalize(reflect(pixelToCamVec, input.vNormal.xyz));
		
		float3 camToPixelVec = (input.positionInWorldSpace.xyz - camPos.xyz);
		float3 camToPixelReflected = normalize(reflect(camToPixelVec, input.vNormal.xyz));
		//float3 camToPixelReflected = normalize(refract(camToPixelVec, input.vNormal.xyz, 3.52)); //Refraction test

		output.vColour = EnvironmentTexture.Sample(mysampler, camToPixelReflected);
		//output.vColour = EnvironmentTexture.Sample(mysampler, pixelToCameraReflected);
	}
		
	//output.vColour = float4(input.vUV.x, input.vUV.y, 0, 0);
	output.vNormal = input.vNormal;
	return output;

}