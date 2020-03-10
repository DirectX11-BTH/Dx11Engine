
//These are the gbuffers which hold pixel by pixel values to be read from.
Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1); 
Texture2D PositionTexture : register(t2);
Texture2D GlowTexture : register(t3);

SamplerState mySampler;

//From CPU to GPU. Sends whatever you need.
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

	bool hasNormalMap;
	bool hasTexture;
	bool environmentMap;
	bool glowingObject;

	bool isWater;
	float4 uvDisplacement;
}

struct VS_OUTPUT //comes from deferredVShader
{
	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 positionInWorldSpace : POSITION;
	float4 vTangent : TANGENT;
};

float4 main(VS_OUTPUT input) : SV_Target0
{

	//Load in values from gbuffer textures/SRVs.
	float4 albedo = ColorTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.vPosition.xy, 0), 0); 
	float4 position = PositionTexture.Load(float3(input.vPosition.xy, 0), 0); 
	float4 glow = GlowTexture.Load(float3(input.vPosition.xy, 0), 0);

	if (albedo.w != 2) //If alpha is one then don't do the light calculation.
	{
		//Calculate light
		float3 surfaceToLightV = normalize(lightPos - position);
		float diffuseStrength = clamp(dot(normal, surfaceToLightV), 0, 1); //Cannot go over 1, and never below 0.
		float ambientStrength = 0.2f; //Constant 'global' light.

		float4 lookVector = normalize(position - camPos); //Specular, camera to position
		float4 reflectionVec = normalize(reflect(float4(surfaceToLightV, 0), normal)); //Specular
		float specStrength = pow(clamp(dot(reflectionVec, lookVector), 0, 1), 32); // 32 being spec exponent
		//

		return ((diffuseStrength + ambientStrength + specStrength) * albedo) + glow; //Final light output
	}
	else
	{
		return albedo + glow;
	}

} 