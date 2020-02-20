Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);

SamplerState mysampler;


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

	row_major float4x4 viewInverseMatrix;
	row_major float4x4 worldInverseMatrix;

	bool hasNormalMap;
	bool hasTexture;
	float4 padding;
	float4 padding2;
}

struct PS_INPUT
{
	float4 vColour : COLOR;
	float4 vPosition : SV_POSITION;
	float4 vUV : UV;
	float4 vNormal : NORMAL;
};

//Width and height of window split by 4.
//const float2 noiseScale = float2(600 / 4.0, 500 / 4.0);

const float radius = 0.5f; //Test radius
const float bias = 0.035f; //Determines how much something needs to be occluded to actually be shaded, raising this makes more of the mesh occluded

float4 main(PS_INPUT input) : SV_Target0 //svTarget being the occlusionBuffer
{
	//get the information from the g-buffers
	float4 albedo = ColorTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 normal = NormalTexture.Load(float3(input.vPosition.xy, 0), 0);
	float4 position = PositionTexture.Load(float3(input.vPosition.xy, 0), 0);
	//position = mul(position, viewMatrix);
	//normal = mul(normal, viewMatrix);

	//This will give you a random vec generated on the cpu, but seems to return the same?
	float3 randomVector = RandomVectorTexture.Sample(mysampler, float2(input.vPosition.x / 1080, input.vPosition.y / 720)).xyz;//RandomNoiseTexture.Load(float3(input.vPosition.xy*noiseScale*input.vUV, 0), 0).xyz; //Getting into the range of our noise and then randomly multiplying by our UV coords


	//create TBN which takes you from tangent space to view space
	float3 tangent = randomVector - normal * dot(randomVector, normal); //Tangent is an orthogonal vector to the normal, i.e. 90 degrees offset
	tangent = normalize(tangent);

	//Now we can get the bitangent which is orthogonal to both vectors (normal and tangent), i.e. a cross product
	float3 bitangent = cross(tangent, normal);

	float3x3 tbn = float3x3(tangent, bitangent, normal.xyz); //Transforms vectors into view space
	//tbn = transpose(tbn); //maybe?

	//time to calculate the occlusion with a loop, looping over all sample points
	float occlusionFactor = 0.0f; //1 is completely occluded, 0 is not occluded

	for (int i = 0; i < 32; i++)
	{
		//RandomVectorTexture.Load(float3((input.vUV.x/1080)*32, (int)(input.vPosition.y/720)*32, 0), 0).xyz
		float3 samplePoint = RandomVectorTexture.Sample(mysampler, float2(input.vPosition.x / 1080, input.vPosition.y / 720)).xyz;
		samplePoint = mul(samplePoint, tbn);// mul(RandomVectorTexture.Sample(mysampler, input.vUV), tbn);
		samplePoint = position + samplePoint * radius;
		float4 offsetFromSample = float4(samplePoint, 1); //Cast to float4 after calculation actual sample point (point to test for occlusion).
		offsetFromSample = mul(offsetFromSample, projMatrix); //Transforming to clip space
		offsetFromSample.xyz = offsetFromSample.xyz / offsetFromSample.w; //Move into normalized device coordinates
		offsetFromSample.xyz = offsetFromSample.xyz * 0.5 + 0.5; //Guarantees 0-1 range.

		offsetFromSample.y = 1.f - offsetFromSample.y; //Invert because UV starts top left

		float4 occludingPosition = PositionTexture.Load(float3(offsetFromSample.xy, 0), 0); //Sample the position we're trying for occlusion
		//occludingPosition = occludingPosition;

		//This will check so that if something is out of range it won't count as occluding. Also smooths cutoff.
		//float rangeInfluence = smoothstep(0.f, 1.0, radius / length(position - occludingPosition)); //If hardcoded to one, it still darkens the entire image. Not supposed to happen. Most likely transformation error.
		float rangeInfluence = 1;
		//Essentially a lerp too.

		if (occludingPosition.z <= samplePoint.z + bias) //if occluder is closer than the sample point then make pixel darker
			occlusionFactor += 1 * rangeInfluence;

	}

	float actualOcclusion = 1.f - (occlusionFactor / 32); //average the occlusion out
	return float4(actualOcclusion, actualOcclusion, actualOcclusion, 1);
	//return debugReturn;
}