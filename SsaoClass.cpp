#include "SsaoClass.h"
using namespace DirectX;

DirectX::XMVECTOR SsaoClass::randomVectors[nrOfVecs];
DirectX::XMVECTOR SsaoClass::noiceVectors[noiseSize];

float SsaoClass::lerp(float a, float b, float t)
{
	return (a * (1.0f - t)) + (b * t);
}

void SsaoClass::generateRandomVectors()
{
	//generate a random set of points to sample from
	srand (time(NULL));
	for (int i = 0; i < nrOfVecs; i++)
	{
		float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2))-1;
		float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2))-1;
		float randZ = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));
		DirectX::XMVECTOR randomVec = XMVectorSet(randX, randY, randZ, 0);
		
		randomVectors[i] = randomVec;
		randomVec = XMVector3Normalize(randomVectors[i]);

		float vecScale = (float)i / (float)nrOfVecs;
		vecScale = lerp(0.1, 1, vecScale*vecScale);
		randomVectors[i] *= vecScale;
	}
}

void SsaoClass::generateNoiseTexture()
{
	for (int i = 0; i < noiseSize; i++)
	{
		float randX = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;
		float randY = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;

		XMVECTOR randomNoiseVector = XMVectorSet(randX, randY, 0, 0);
		randomNoiseVector = XMVector3Normalize(randomNoiseVector);
		noiceVectors[i] = randomNoiseVector;
		
	}
}