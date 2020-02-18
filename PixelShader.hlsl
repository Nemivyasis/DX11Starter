

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
	float3 cameraPosition;
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
	float3 worldPos		: POSITION;
};

// Calculates the specular reflection using the Phong lighting
// model, given a surface normal, a direction FROM the light,
// a direction TO the camera (from the surface) and a specular 
// exponent value (the shininess of the surface)
float SpecularPhong(float3 normal, float3 lightDir, float3 toCamera, float specExp)
{
	// Calculate light reflection vector
	float3 refl = reflect(normalize(lightDir), normal);

	// 1. Get the dot product between the perfect reflection of the light
	//    and the vector to the camera, as that tells us how "close" we 
	//    are to seeing the perfect reflection.  
	// 2. Saturate to ensure it doesn't go negative on the back of the object
	// 3. Raise to a power to create a very quick "falloff" (so the highlight
	//    appears smaller if the object is shinier)
	return pow(saturate(dot(refl, toCamera)), specExp);
}

float3 CalculateLighting(DirectionalLight l, VertexToPixel input) {

	//calculate specular
	float3 toCamera = normalize(cameraPosition - input.worldPos);
	float spec = SpecularPhong(input.normal, l.direction, toCamera, 64.0f); // do not use the negative normalized ldirection

	//calculate diffuse
	float lightAmount = saturate(dot(input.normal, -normalize(l.direction)));

	return lightAmount * l.diffuseColor + l.ambientColor + spec * l.diffuseColor;
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

	float3 lighting = CalculateLighting(light, input)
		+ CalculateLighting(light2, input) + CalculateLighting(light3, input);

	return float4(lighting, 1) * input.color;
}