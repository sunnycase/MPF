
struct VertexShaderInput
{
	float3 Position : POSITION;
	float2 Normal : NORMAL;
};

struct PixelShaderInput
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};