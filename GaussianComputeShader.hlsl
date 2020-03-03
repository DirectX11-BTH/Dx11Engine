Texture2D<float4> textureToBlur : register(t0);
RWTexture2D<float4> outputTarget : register(u0);

#define size_x 36
#define size_y 20

//x^2 * y^2 = 262 144

//36 x 20 groups
// 36 x 20 per group
// = 720 x 720 pixels
//z = 1

static const float gaussianKernel[7][7] = {
	0 * 1 / (1003), 0 * 1 / (1003), 1 * 1 / (1003), 2 * 1 / (1003), 1 * 1 / (1003), 0 * 1 / (1003), 0 * 1 / (1003),
	0 * 1 / (1003), 3 * 1 / (1003), 13 * 1 / (1003), 22 * 1 / (1003), 13 * 1 / (1003), 3 * 1 / (1003), 0 * 1 / (1003),
	1 * 1 / (1003), 13 * 1 / (1003), 59 * 1 / (1003), 97 * 1 / (1003), 59 * 1 / (1003), 13 * 1 / (1003), 1 * 1 / (1003),
	2 * 1 / (1003), 22 * 1 / (1003), 97 * 1 / (1003), 159 * 1 / (1003), 97 * 1 / (1003), 22 * 1 / (1003), 2 * 1 / (1003),
	1 * 1 / (1003), 13 * 1 / (1003), 59 * 1 / (1003), 97 * 1 / (1003), 59 * 1 / (1003), 13 * 1 / (1003), 1 * 1 / (1003),
	0 * 1 / (1003), 3 * 1 / (1003), 13 * 1 / (1003), 22 * 1 / (1003), 13 * 1 / (1003), 3 * 1 / (1003), 0 * 1 / (1003),
	0 * 1 / (1003), 0 * 1 / (1003), 1 * 1 / (1003), 2 * 1 / (1003), 1 * 1 / (1003), 0 * 1 / (1003), 0 * 1 / (1003)
};

[numthreads(size_x, size_y, 1)] //Each group contains n x m x 1 threads.
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	//do the magic thing here, start at the top left of the kernel
	int2 pixelLocation = DispatchThreadID.xyz - int3(3, 3, 0); //pixel coords
	float4 outputColor = float4(0, 0, 0, 0); //Start with zero

	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			outputColor += textureToBlur.Load(float3(pixelLocation + int3(x, y, 0), 0)) * gaussianKernel[x][y];
		}
	}
	
	outputTarget[DispatchThreadID.xy] = float4(1, 0, 0, 1);//outputColor;
};
