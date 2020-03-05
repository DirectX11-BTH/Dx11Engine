Texture2D<float4> textureToBlur : register(t0);
RWTexture2D<float4> outputTarget : register(u0);
Texture2D gaussianKernelTexture : register(t1);

#define size_x 18
#define size_y 18

//x^2 * y^2 = 262 144

//36 x 20 groups
// 36 x 20 per group
// = 720 x 720 pixels
//z = 1

static const float kernelSize = 25;

[numthreads(size_x, size_y, 1)] //Each group contains n x m x 1 threads.
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	//do the magic thing here, start at the top left of the kernel
	int2 pixelLocation = DispatchThreadID.xyz - int3(3, 3, 0); //pixel coords
	float4 outputColor = float4(0, 0, 0, 0); //Start at zero

	for (int y = 0; y < kernelSize; y++)
	{
		for (int x = 0; x < kernelSize; x++)
		{
			float gaussWeight = gaussianKernelTexture.Load(float3(x, y, 0), 0).x;
			outputColor += textureToBlur.Load(float3(pixelLocation + int3(x-10, y-10, 0), 0)) * gaussWeight;//gaussianKernelTexture[x][y].x; //Weight together adjacent pixel based on kernel weights
		}
	}
	outputTarget[DispatchThreadID.xy] = outputColor;
};
