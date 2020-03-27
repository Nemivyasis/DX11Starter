#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__
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

struct PointLight
{
	float3 ambientColor;
	float padding1;
	float3 diffuseColor;
	float padding2;
	float3 position;
	float padding3;
};

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
	float2 uv			: TEXCOORD;
};

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixelNormalMap
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
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
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

float3 CalculateLighting(float3 dir, float3 ambientColor, float3 diffuseColor, float3 cameraPosition, float reflectivity, float3 worldPos, float3 normal) {

	//calculate specular
	float3 toCamera = normalize(cameraPosition - worldPos);
	float spec = 0;

	if (reflectivity > 0) {
		spec = SpecularPhong(normal, dir, toCamera, reflectivity); // do not use the negative normalized ldirection
	}

	//calculate diffuse
	float lightAmount = saturate(dot(normal, -normalize(dir)));

	return lightAmount * diffuseColor + ambientColor + spec;
}

float3 CalculateLightingDirectional(DirectionalLight l, float3 cameraPosition, float reflectivity, float3 worldPos, float3 normal) {
	return CalculateLighting(l.direction, l.ambientColor, l.diffuseColor, cameraPosition, reflectivity, worldPos, normal);
}

float3 CalculateLightingPoint(PointLight l, float3 cameraPosition, float reflectivity, float3 worldPos, float3 normal) {
	float3 lightDir = normalize(worldPos - l.position);
	return CalculateLighting(lightDir, l.ambientColor, l.diffuseColor, cameraPosition, reflectivity, worldPos, normal);
}

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
};
#endif