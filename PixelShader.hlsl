

//Directional Light struct
struct DirectionalLight 
{
	float3 ambientColor;
	float padding1;
	float3 diffuseColor;
	float padding2;
	float3 direction;
	float padding3;
};

cbuffer ExternalData : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
	DirectionalLight light3;
}

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
};


float3 CalculateLighting(DirectionalLight l, float3 normal) {
	float3 lDir = -normalize(l.direction);
	float lightAmount = saturate(dot(normal, lDir));
	return lightAmount * l.diffuseColor + l.ambientColor;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	input.normal = normalize(input.normal);

	float3 lighting = CalculateLighting(light, input.normal) 
		+ CalculateLighting(light2, input.normal) + CalculateLighting(light3, input.normal);

	return float4(lighting, 1) * input.color;
}