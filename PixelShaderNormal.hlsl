#include<ShaderEverything.hlsli>

cbuffer ExternalData : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
	DirectionalLight light3;
	PointLight light4;
	float reflectivity;
	float3 cameraPosition;
}

Texture2D diffuseTexture	: register(t0);
Texture2D normalMap			: register(t1);
SamplerState samplerOptions	: register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixelNormalMap input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
float3 unpackedNormal = normalMap.Sample(samplerOptions, input.uv).rgb * 2 - 1;
float3 N = normalize(input.normal);
float3 T = normalize(input.tangent);
T = normalize(T - N * dot(T, N));
float3 B = cross(T, N);
float3x3 TBN = float3x3(T, B, N);

input.normal = normalize(mul(unpackedNormal, TBN));

float3 lighting = CalculateLightingPoint(light4, cameraPosition, reflectivity, input.worldPos, input.normal)
+ CalculateLightingDirectional(light, cameraPosition, reflectivity, input.worldPos, input.normal)
+ CalculateLightingDirectional(light2, cameraPosition, reflectivity, input.worldPos, input.normal)
+ CalculateLightingDirectional(light3, cameraPosition, reflectivity, input.worldPos, input.normal);

	float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv).rgb;
	return float4(lighting, 1) * input.color * float4(surfaceColor, 1);
}