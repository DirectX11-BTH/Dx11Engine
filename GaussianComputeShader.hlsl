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

0.004391	0.004402	0.004412	0.00442	0.004426	0.00443	0.004433	0.004434	0.004433	0.00443	0.004426	0.00442	0.004412	0.004402	0.004391
0.004402	0.004414	0.004423	0.004431	0.004438	0.004442	0.004445	0.004446	0.004445	0.004442	0.004438	0.004431	0.004423	0.004414	0.004402
0.004412	0.004423	0.004433	0.004441	0.004447	0.004452	0.004454	0.004455	0.004454	0.004452	0.004447	0.004441	0.004433	0.004423	0.004412
0.00442		0.004431	0.004441	0.004449	0.004455	0.00446	0.004462	0.004463	0.004462	0.00446	0.004455	0.004449	0.004441	0.004431	0.00442
0.004426	0.004438	0.004447	0.004455	0.004462	0.004466	0.004469	0.00447	0.004469	0.004466	0.004462	0.004455	0.004447	0.004438	0.004426
0.00443		0.004442	0.004452	0.00446	0.004466	0.004471	0.004473	0.004474	0.004473	0.004471	0.004466	0.00446	0.004452	0.004442	0.00443
0.004433	0.004445	0.004454	0.004462	0.004469	0.004473	0.004476	0.004477	0.004476	0.004473	0.004469	0.004462	0.004454	0.004445	0.004433
0.004434	0.004446	0.004455	0.004463	0.00447	0.004474	0.004477	0.004478	0.004477	0.004474	0.00447	0.004463	0.004455	0.004446	0.004434
0.004433	0.004445	0.004454	0.004462	0.004469	0.004473	0.004476	0.004477	0.004476	0.004473	0.004469	0.004462	0.004454	0.004445	0.004433
0.00443		0.004442	0.004452	0.00446	0.004466	0.004471	0.004473	0.004474	0.004473	0.004471	0.004466	0.00446	0.004452	0.004442	0.00443
0.004426	0.004438	0.004447	0.004455	0.004462	0.004466	0.004469	0.00447	0.004469	0.004466	0.004462	0.004455	0.004447	0.004438	0.004426
0.00442		0.004431	0.004441	0.004449	0.004455	0.00446	0.004462	0.004463	0.004462	0.00446	0.004455	0.004449	0.004441	0.004431	0.00442
0.004412	0.004423	0.004433	0.004441	0.004447	0.004452	0.004454	0.004455	0.004454	0.004452	0.004447	0.004441	0.004433	0.004423	0.004412
0.004402	0.004414	0.004423	0.004431	0.004438	0.004442	0.004445	0.004446	0.004445	0.004442	0.004438	0.004431	0.004423	0.004414	0.004402
0.004391	0.004402	0.004412	0.00442	0.004426	0.00443	0.004433	0.004434	0.004433	0.00443	0.004426	0.00442	0.004412	0.004402	0.004391
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
