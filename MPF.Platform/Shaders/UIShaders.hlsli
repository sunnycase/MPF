
struct VertexShaderInput
{
	float3 Position : POSITION;
	float2 Normal : NORMAL;
	float2 ParamFormValue : TEXCOORD0;
	float SegmentType : TEXCOORD1;
};

struct PixelShaderInput
{
	float4 Position : POSITION;
	float3 NormalAndThickness : TEXCOORD0;
	float4 Color : COLOR;
	float2 ParamFormValue : TEXCOORD1;
	int SegmentType : TEXCOORD2;
};

#define	ST_Linear				0
#define	ST_QuadraticBezier		1
#define	ST_CubicBezier			2
#define	ST_Arc					3