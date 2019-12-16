/*
	Authors: Jakob Lidborn, Ghazi Hicheri, Christian Leo
	December 6th 2019
*/

/*cbuffer VS_CONSTANT_BUFFER
{
	Matrix rotationMatrix;
}*/

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
	float4 vInterpolatedPosition : POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT Output;

	//float4 pos = float4(input.vPosition.xy, input.vPosition.z, 1.0);
	//Output.vPosition = float4(mul(pos,rotationMatrix)); //Ignore W
	Output.vPosition = float4(input.vPosition, 1);
	//Output.vInterpolatedPosition = float4(mul(pos, rotationMatrix));


	//Output.vNormal = normalize(mul(float4(normalize(input.vNormal), 1), rotationMatrix));

	Output.vColour = input.vColour;
	Output.vUV = float4(input.vUV, 1, 1);
	//Output.vNormal = input.vNormal;
	return Output;
}