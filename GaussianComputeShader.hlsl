Texture2D<float4> textureToBlur : register(t0);
RWTexture2D<float4> outputTarget : register(u0);

#define size_x 18
#define size_y 18

//x^2 * y^2 = 262 144

//36 x 20 groups
// 36 x 20 per group
// = 720 x 720 pixels
//z = 1

static const float gaussianKernel[7][7] = {
0.000036,	0.000363,	0.001446,	0.002291,	0.001446,	0.000363,	0.000036,
0.000363,	0.003676,	0.014662,	0.023226,	0.014662,	0.003676,	0.000363,
0.001446,	0.014662,	0.058488,	0.092651,	0.058488,	0.014662,	0.001446,
0.002291,	0.023226,	0.092651,	0.146768,	0.092651,	0.023226,	0.002291,
0.001446,	0.014662,	0.058488,	0.092651,	0.058488,	0.014662,	0.001446,
0.000363,	0.003676,	0.014662,	0.023226,	0.014662,	0.003676,	0.000363,
0.000036,	0.000363,	0.001446,	0.002291,	0.001446,	0.000363,	0.000036
};

[numthreads(size_x, size_y, 1)] //Each group contains n x m x 1 threads.
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	//do the magic thing here, start at the top left of the kernel
	int2 pixelLocation = DispatchThreadID.xyz - int3(3, 3, 0); //pixel coords
	float4 outputColor = float4(0, 0, 0, 0); //Start at zero

	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			outputColor += textureToBlur.Load(float3(pixelLocation + int3(x, y, 0), 0)) * gaussianKernel[x][y]; //Weight together adjacent pixel based on kernel weights
		}
	}
	outputTarget[DispatchThreadID.xy] = outputColor;
};
