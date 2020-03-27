#include<ShaderEverything.hlsli>

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float4x4 worldMatrix;
	float4x4 projectionMatrix;
	float4x4 viewMatrix;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixelNormalMap main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixelNormalMap output;

	// Here we're essentially passing the input position directly through to the next
	// stage (rasterizer), though it needs to be a 4-component vector now.  
	// - To be considered within the bounds of the screen, the X and Y components 
	//   must be between -1 and 1.  
	// - The Z component must be between 0 and 1.  
	// - Each of these components is then automatically divided by the W component, 
	//   which we're leaving at 1.0 for now (this is more useful when dealing with 
	//   a perspective projection matrix, which we'll get to in future assignments).
	matrix wvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
	output.position = mul(wvp, float4(input.position, 1.0f));

	output.worldPos = mul(worldMatrix, float4(input.position, 1.0f)).xyz;

	// Pass the color through 
	// - The values will be interpolated per-pixel by the rasterizer
	// - We don't need to alter it here, but we do need to send it to the pixel shader
	output.color = colorTint;

	//transform normal ignoring translation
	//TODO Make world the inverse transpose of world (calculate in C++ and pass in as constant buffer)
	output.normal = mul((float3x3)worldMatrix, input.normal);

	output.tangent = mul((float3x3)worldMatrix, input.tangent);

	output.uv = input.uv;
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}