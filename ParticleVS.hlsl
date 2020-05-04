//constant buffer for data being passed in
cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

//Describes the input data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float4 color		: COLOR;
};

//Defines output
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
	float4 color		: TEXCOORD1;
};

VertexToPixel main(VertexShaderInput input) 
{
	//set up output object
	VertexToPixel output;

	//calculate position
	matrix viewProj = mul(projection, view);
	output.position = mul(viewProj, float4(input.position, 1.0f));

	//pass uv through (both coord and color
	output.uv = input.uv;
	output.color = input.color;

	return output;
}