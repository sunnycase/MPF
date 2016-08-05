#include "UIShaders.hlsli"

#define	ST_Linear				0
#define	ST_QuadraticBezier		1
#define	ST_CubicBezier			2
#define	ST_Arc					3

float GetQuadraticBezierSignedDistance(float2 px, float2 py, float2 value)
{
	float fx = (2 * value.x)*px.x - px.y;
	float fy = (2 * value.x)*py.x - py.y;
	// Signed distance  
	return (value.x * value.x - value.y) / sqrt(fx*fx + fy*fy);
}

float GetArcSignedDistance(float2 px, float2 py, float2 value)
{
	float fx = (2 * value.x)*px.x + (2 * value.y)*px.y;
	float fy = (2 * value.x)*py.x + (2 * value.y)*py.y;
	// Signed distance  
	return (value.x * value.x + value.y * value.y - 1.f) / sqrt(fx*fx + fy*fy);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	int segmentType = input.SegmentType;
	if (segmentType == ST_Linear);
	else
	{
		float thickness = input.NormalAndThickness.z;
		float2 px = ddx(input.ParamFormValue);
		float2 py = ddy(input.ParamFormValue);
		float sd;
		if (segmentType == ST_QuadraticBezier)
			sd = GetQuadraticBezierSignedDistance(px, py, input.ParamFormValue);
		else if (segmentType == ST_Arc)
			sd = GetArcSignedDistance(px, py, input.ParamFormValue);
		float alpha = thickness - abs(sd);
		clip(alpha);
		return input.Color + float4(1.f, 1.f, 1.f, 1.f) * (1.f - saturate(alpha));
	}
	return input.Color;
}