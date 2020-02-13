#pragma once
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>
#include <math.h>
#include <vector>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <assert.h>
#include <cmath>

class SsaoClass
{
private:
	static float lerp(float a, float b, float t);
	static const int nrOfVecs = 32;
	static const int noiseSize = 16;
public:
	//static int nrOfVecs; //Can not go past 32
	static DirectX::XMVECTOR randomVectors[nrOfVecs];
	static DirectX::XMVECTOR noiceVectors[noiseSize];
	static void generateRandomVectors();
	static void generateNoiseTexture();
};