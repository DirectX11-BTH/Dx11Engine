/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/

//Buffer members from the CPU.
cbuffer VS_CONSTANT_BUFFER
{
	row_major float4x4 worldViewProjectionMatrix;
	row_major float4x4 worldMatrix;
	row_major float4x4 viewMatrix;
	row_major float4x4 projMatrix;

	row_major float4x4 viewInverseMatrix;
	row_major float4x4 worldInverseMatrix;
}

struct VS_INPUT
{
	float3 vPosition : POSITION;
	float4 vColour : COLOR;
	float2 vUV : UV;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
};

struct VS_OUTPUT //Outputs to ordinary first pass pixel shader.
{
	float4 vPosition : SV_POSITION;	
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT Output;

	//Transform things for the pixel shader to output to the deferred render buffers.
	//We store things in world space.
	Output.positionInWorldSpace = mul(float4(input.vPosition, 1), worldMatrix);
	Output.vNormal = normalize(mul(float4(normalize(input.vNormal), 0), worldMatrix));
	Output.vTangent= normalize(mul(float4(normalize(input.vTangent), 0), worldMatrix));
	Output.vPosition = mul(float4(input.vPosition, 1), worldViewProjectionMatrix);
	Output.vColour = input.vColour;
	Output.vUV = float4(input.vUV, 1, 1);

	return Output;
}