
struct VertexShaderInput
{
	float3 Position : SV_Position;
	float3 Normal : NORMAL;
	float3 TexCoord : TEXCOORD0;
};

struct PixelShaderInput
{
	float4 Position : SV_Position;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float3 TexCoord : TEXCOORD0;
};