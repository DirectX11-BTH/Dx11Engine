Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1); 
Texture2D PositionTexture : register(t2);

//SSAO magic
Texture2D RandomVectorTexture : register(t3);
Texture2D RandomNoiseTexture : register(t4);
//


cbuffer PS_CONSTANT_BUFFER
{
	float4 lightPos;
	float4 ambientMeshColor;
	float4 diffueMeshColor;
	float4 specularMeshColor;
	float4 camPos;
	row_major float4x4 worldViewProjectionMatrix;
	float4 specularExponent; //Only use x value
	float2 noiseScale;

	row_major float4x4 worldMatrix;
	row_major float4x4 viewMatrix;
	row_major float4x4 projMatrix;
}

struct VS_OUTPUT
{
	float4 vColour : COLOR;
	float4 vPosition : SV_POSITION;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
};

float4 main(VS_OUTPUT input) : SV_Target0
{
	
	float4 albedo = ColorTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 position = PositionTexture.Load(float3(input.vPosition.xy, 0), 0);


	//float3 surfaceToLightV = normalize(mul(lightPos - position, worldViewProjectionMatrix)); //make to worldmatrix
	float3 surfaceToLightV = normalize(lightPos - position);
	float diffuseStrength = clamp(dot(normal, surfaceToLightV), 0, 1);
	float ambientStrength = 0.2f;

	float4 lookVector = normalize(position - camPos); //Specular
	float4 reflectionVec = normalize(reflect(float4(surfaceToLightV, 0), normal)); //Specular
	float specStrength = pow(clamp(dot(reflectionVec, lookVector), 0, 1), 100); // 10 being spec exponent

	/*
		vec3 rvec = texture(uTexRandom, vTexcoord * uNoiseScale).xyz * 2.0 - 1.0;
	   vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	   vec3 bitangent = cross(normal, tangent);
	   mat3 tbn = mat3(tangent, bitangent, normal);
	*/
	
	// p -- the point we are computing the ambient occlusion for.
	// n -- normal vector at p.
	// q -- a random offset from p. = randomVector
	// r -- a potential occluder that might occlude p.
	float3 p = position.xyz;
	float3 n = normal.xyz;
	float3 q = RandomVectorTexture.Load(float3(input.vPosition.xy * (1/32), 0), 0).xyz; //Always within 32x32 interval

	int samplePoints = 32;
	float occlSum = 0.0f;
	float occlusionRadius = 1.0f;
	for (int i = 0; i < samplePoints; i++)
	{
		float3 offset = q;
		float infrontOrBack = sign(dot(offset, n)); //positive if in front, negative if back
		q = p + infrontOrBack * occlusionRadius * offset;
		float4 projectionQ = mul(float4(q, 1.0f), viewMatrix);
		projectionQ /= projectionQ.w;

		float rz = PositionTexture.Load(float3(projectionQ.xy, 0), 0).z;
		float3 r = (rz / q.z) * q;

		float zDistance = p.z - r.z;

		float occlusionMultiplier = 0.0f;
		if (zDistance > 0.001f)
		{
			float fadeLength = 1.0f - 0.2f;
			occlusionMultiplier = saturate((1.0f - zDistance)/fadeLength);
		}

		float dp = max(dot(n, normalize(r - p)), 0.0f);
		float occlusion = dp; //* occlusionMultiplier;
	
		occlSum += occlusion;
	}
	occlSum / (float)samplePoints;
	float occlAcc = 1.0f - occlSum;
	//return (diffuseStrength + (ambientStrength) + specStrength) * albedo;// * (1.f-occl/32); 
	return albedo * (ambientStrength-occlAcc);
}