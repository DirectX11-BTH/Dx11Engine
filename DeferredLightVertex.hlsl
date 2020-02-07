
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
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.vPosition = float4(input.vPosition, 1);
	output.vColour = input.vColour;
	output.vUV = float4(input.vUV, 1, 1);
	output.vNormal = float4(input.vNormal, 0);
	
	
	return output;
}