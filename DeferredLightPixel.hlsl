Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1); 
Texture2D PositionTexture : register(t2);

cbuffer PS_CONSTANT_BUFFER
{
	float4 lightPos;
	float4 ambientMeshColor;
	float4 diffueMeshColor;
	float4 specularMeshColor;
	float4 camPos;
	row_major float4x4 worldViewProjectionMatrix;
	float4 specularExponent; //Only use x value
}

struct VS_OUTPUT
{
	float4 vColour : COLOR;
	float4 vPosition : SV_POSITION;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 albedo = ColorTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 position = NormalTexture.Load(float3(input.vPosition.xy, 0), 0);


	float3 surfaceToLightV = normalize(mul(lightPos - position, worldViewProjectionMatrix));
	float diffuseStrength = clamp(dot(normal, surfaceToLightV), 0, 1);
	float ambientStrength = 0.2f;

	float4 lookVector = normalize(camPos - position); //Specular
	float4 reflectionVec = normalize(reflect(float4(surfaceToLightV, 0), normal)); //Specular
	float specStrength = pow(clamp(dot(reflectionVec, lookVector), 0, 1), 10); // 10 being spec exponent

	return  float4(1, 1, 1, 1);//(diffuseStrength + ambientStrength + specStrength) * albedo; 
}
//To do:
//Render full screen quad to active all pixels
//Compile Shaders
//