
struct VertexShaderInput
{
	float3 Position : SV_Position;
	float2 Data1 : NORMAL;
	float2 Data2 : TEXCOORD0;
	float SegmentType : TEXCOORD1;
	float2 Data3 : TEXCOORD2;
	float2 Data4 : TEXCOORD3;
	float2 Data5 : TEXCOORD4;
};

struct PixelShaderInput
{
	float4 Position : SV_Position;
	nointerpolation float3 NormalAndThickness : TEXCOORD0;
	float4 Color : COLOR;
	float2 ParamFormValue : TEXCOORD1;
	int SegmentType : TEXCOORD2;
};

#define	ST_Linear				0
#define	ST_QuadraticBezier		1
#define	ST_CubicBezier			2
#define	ST_Arc					3