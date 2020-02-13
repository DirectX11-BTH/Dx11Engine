/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/

cbuffer VS_CONSTANT_BUFFER
{
	row_major float4x4 worldViewProjectionMatrix;
	row_major float4x4 worldMatrix;
}

struct VS_INPUT
{
	float3 vPosition : POSITION;
	float4 vColour : COLOR;
	float2 vUV : UV;
	float3 vNormal : NORMAL;
};

struct VS_OUTPUT
{
	float4 vColour : COLOR;
	float4 vPosition : SV_POSITION;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT Output;

	//Move everything to view for SSAO, not world <----------------------------------------
	//In light pass, multiply light with view to do ssao in light pass
	//Also need to send position in world space to pixel shader, output THAT to gbuffer

	/////////Output.vPosition = normalize(mul(float4(normalize(input.vPosition), 1), worldMatrix));
	Output.vPosition = mul(float4(input.vPosition, 1), worldViewProjectionMatrix);
	Output.positionInWorldSpace = mul(float4(input.vPosition, 1), worldMatrix);

	Output.vNormal = normalize(mul(float4(normalize(input.vNormal), 0), worldMatrix));

	Output.vColour = input.vColour;
	Output.vUV = float4(input.vUV, 1, 1);
	//Output.vInterpolatedPosition = mul(float4(input.vPosition, 1), worldViewProjectionMatrix);
	//Output.vNormal = input.vNormal;
	return Output;
}