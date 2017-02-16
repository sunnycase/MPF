
struct VertexShaderInput
{
	float3 Position : SV_Position;
};

struct PixelShaderInput
{
	float4 Position : SV_Position;
	float4 Color : COLOR;
};