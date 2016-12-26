
struct VertexShaderInput
{
	float3 Position : SV_Position;
};

struct PixelShaderInput
{
	float4 Position : SV_Position;
	float4 Color : COLOR;
};

#define	ST_Linear				0
#define	ST_QuadraticBezier		1
#define	ST_CubicBezier			2
#define	ST_Arc					3