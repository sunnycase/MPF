#include "UIShaders.hlsli"

struct tagWorldViewProj
{
	matrix worldMat;
	matrix viewMat;
	matrix projMat;
};
extern tagWorldViewProj WVP: register(c0);

struct tagStorkeInfo
{
	float thickness;
	float padding[3];
};
extern tagStorkeInfo stroke : register(c12);
extern float4 color : register(c16);

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.Position, 1.f);
	pos.xy += stroke.thickness * input.Normal / 2.f;

	pos = mul(pos, WVP.worldMat);
	pos = mul(pos, WVP.viewMat);
	pos = mul(pos, WVP.projMat);

	output.Position = pos;
	output.Color = color;
	return output;
}