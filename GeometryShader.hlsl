cbuffer GS_CONSTANT_BUFFER
{
	float4 camPos;
};

struct GS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

struct GS_INPUT
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

[maxvertexcount(3)]
void main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> output)
{
	float3 zeroToOne = (input[1].positionInWorldSpace - input[0].positionInWorldSpace).xyz;
	float3 zeroToTwo = (input[2].positionInWorldSpace - input[0].positionInWorldSpace).xyz;
	float3 normal = normalize(cross(zeroToTwo, zeroToOne));

	float3 camToPoint = normalize(input[0].positionInWorldSpace - camPos.xyz);
	if (dot(normal, camToPoint) > 0.f)
	{
		for (uint i = 0; i < 3; i++)
		{
			GS_OUTPUT element;

			element.vPosition = input[i].vPosition;
			element.vColour = input[i].vColour;
			element.vUV = input[i].vUV;
			element.vNormal = input[i].vNormal;
			element.positionInWorldSpace = input[i].positionInWorldSpace;
			element.vTangent = input[i].vTangent;

			output.Append(element);
		}
	}

	/*for (int i = 0; i < 3; i++)
		output.Append(input[i]);*/
	
}