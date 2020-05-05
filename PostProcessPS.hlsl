cbuffer externalData : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int blurAmount;
}

// Defines the input to this pixel shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and such
Texture2D pixels			: register(t0);
SamplerState samplerOptions	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 totalColor = float4(0,0,0,0);
	uint numSamples = 0;

	for (int x = -blurAmount; x <= blurAmount; x++)
	{
		float2 uv = input.uv + float2(x * pixelWidth * input.position.z, 0);
		totalColor += pixels.Sample(samplerOptions, uv);

		numSamples++;
	}

	return totalColor / numSamples;
}