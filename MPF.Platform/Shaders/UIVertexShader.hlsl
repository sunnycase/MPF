#include "UIShaders.hlsli"

struct WorldViewProjection
{
	matrix WorldView;
	matrix Projection;
};
extern WorldViewProjection _wvp: register(c0);

struct tagStorkeInfo
{
	float thickness;
	float padding[3];
};
extern tagStorkeInfo _stroke : register(c12);
extern float4 _color : register(c16);

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.Position, 1.f);

	pos = mul(pos, _wvp.WorldView);
	pos.xy += _stroke.thickness * input.Normal / 2.f;
	pos = mul(pos, _wvp.Projection);

	output.Position = pos;
	output.NormalAndThickness = float3(input.Normal, _stroke.thickness);
	output.Color = _color;
	output.ParamFormValue = input.ParamFormValue;
	output.SegmentType = input.SegmentType;
	return output;
}