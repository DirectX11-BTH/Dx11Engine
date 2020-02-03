/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	February 1st 2020
*/

cbuffer VS_CONSTANT_BUFFER
{
	row_major float4x4 worldViewProjectionMatrix;
	row_major float4x4 worldMatrix;
}

struct VS_INPUT
{
	float3 vNormal : NORMAL;
	float3 vPosition : POSITION;
	float4 vColour : COLOR;
	float2 vUV : UV;
};

struct VS_OUTPUT
{
	float4 vNormal : NORMAL;
	float4 vPosition : SV_POSITION;
	float4 vUV : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT Output;
	Output.vPosition = mul(float4(input.vPosition, 1), worldViewProjectionMatrix);
	Output.vNormal = normalize(mul(normalize(input.vNormal), worldMatrix));
	Output.vUV = float4(input.vUV, 1, 1);

	return Output;
}