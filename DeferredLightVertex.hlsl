
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

VS_OUTPUT main(float4 VS_INPUT : POSITION) : SV_POSITION
{
	VS_OUTPUT output;
	output.vColour = float4(input.vColour,1);
	output.vPosition = float4(input.vPosition, 1);
	output.vNormal = float4(input.vNormal, 0);
	output.vUV = float4(input.vUV, 1, 1);
	
	return output;
}