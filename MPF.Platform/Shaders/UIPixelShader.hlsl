#include "UIShaders.hlsli"

#define	ST_Linear				0
#define	ST_QuadraticBezier		1
#define	ST_CubicBezier			2
#define	ST_Arc					3

void ClipForQuadraticBezier(in PixelShaderInput input)
{
	float thickness = input.NormalAndThickness.z;
	float2 px = ddx(input.ParamFormValue);
	float2 py = ddy(input.ParamFormValue);
	// Chain rule  
	float fx = (2 * input.ParamFormValue.x)*px.x - px.y;
	float fy = (2 * input.ParamFormValue.x)*py.x - py.y;
	// Signed distance  
	float sd = (input.ParamFormValue.x * input.ParamFormValue.x - input.ParamFormValue.y) / sqrt(fx*fx + fy*fy);
	float alpha = thickness / 2 - abs(sd);
	clip(alpha);
}

void ClipForArc(in PixelShaderInput input)
{
	//float thickness = input.NormalAndThickness.z;
	//float2 px = ddx(input.ParamFormValue);
	//float2 py = ddy(input.ParamFormValue);
	//// Chain rule  
	//float fx = (2 * input.ParamFormValue.x)*px.x - px.y;
	//float fy = (2 * input.ParamFormValue.x)*py.x - py.y;
	//// Signed distance  
	//float sd = (input.ParamFormValue.x * input.ParamFormValue.x - input.ParamFormValue.y) / sqrt(fx*fx + fy*fy);
	//float alpha = thickness / 2 - abs(sd);
	//clip(alpha);
	clip(-1);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
if (input.SegmentType == ST_QuadraticBezier)
ClipForQuadraticBezier(input);
else if (input.SegmentType == ST_Arc)
ClipForArc(input);
return input.Color;
}