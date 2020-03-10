
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

struct VS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

//Pure passthrough, world matrix is indentity matrix when doing this.
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.positionInWorldSpace = mul(float4(input.vPosition, 1), worldMatrix);
	output.vPosition = float4(input.vPosition, 1);
	output.vColour = input.vColour;
	output.vUV = float4(input.vUV, 0, 1);
	output.vNormal = float4(input.vNormal, 0);
	output.vTangent = float4(input.vTangent, 0);
	
	return output;
}