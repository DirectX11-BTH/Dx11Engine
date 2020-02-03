/*
	After DeferredVertex and DeferredPixel has written into their buffers, we now need to do the second pass.
	This includes reading data through this vertex shader and then feeding it to the deferredLight pixel shader.

	We're technically only reading 2D data here, as normals are stored in one of first pass buffers, so therefore
	we only need to be passing positions.
*/


cbuffer VS_CONSTANT_BUFFER
{
	row_major float4x4 worldViewProjectionMatrix;
	row_major float4x4 worldMatrix;
}

struct INPUT
{
	float4 pos : POSITION;
	float2 textureCoord : TEXCOORD0;
};

struct OUTPUT_VS
{
	float4 pos : SV_POSITION;
	float2 textureCoord : TEXCOORD0;
};

OUTPUT_VS main(INPUT input) 
{
	OUTPUT_VS output;
	output.pos = mul(input.pos, worldViewProjectionMatrix);
	output.textureCoord = input.textureCoord;

	return output;
}