Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1); 
Texture2D PositionTexture : register(t2);

SamplerState mySampler;


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
}

struct VS_OUTPUT
{

	float4 vPosition : SV_POSITION;
	float4 vColour : COLOR;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
	float4 vTangent : TANGENT;
};


float4 main(VS_OUTPUT input) : SV_Target0
{
	//SSAO - move things to view space, world -> view -> proj -> perspective division
	float4 lightPosViewspace = lightPos;//mul(lightPos, viewMatrix); //Light pos already starts in world
	float4 camPosInView = camPos;//mul(camPos, viewMatrix); //Same with camPos
	//

	float4 albedo = ColorTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.vPosition.xy, 0), 0); //in view space
	float4 position = PositionTexture.Load(float3(input.vPosition.xy, 0), 0); //in view space now, due to SSAO

	//position = mul(position, viewInverseMatrix);
	//normal = normalize(mul(normal, viewInverseMatrix));


	//float3 surfaceToLightV = normalize(mul(lightPos - position, worldViewProjectionMatrix)); //make to worldmatrix
	float3 surfaceToLightV = normalize(lightPosViewspace - position);
	float diffuseStrength = clamp(dot(normal, surfaceToLightV), 0, 1);
	float ambientStrength = 0.2f;

	//mul(surfaceToLightV, viewInverseMatrix);

	float4 lookVector = normalize(position - camPosInView); //Specular
	float4 reflectionVec = normalize(reflect(float4(surfaceToLightV, 0), normal)); //Specular
	float specStrength = pow(clamp(dot(reflectionVec, lookVector), 0, 1), 32); // 100 being spec exponent

	//return ssaoOcclusion;
	//return (float4(0.6, 0.6, 0.6, 0.6) * ssaoOcclusion.x)+(albedo*0.1);//
	//return (diffuseStrength + (ambientStrength*ssaoOcclusion.x) + specStrength) * albedo;
	return (diffuseStrength + ambientStrength + specStrength) * albedo;
	//return normal;
	//return (specStrength + (ambientStrength*0.5)) * albedo;
	//return float4(specStrength, 0, 0, 0);
} 