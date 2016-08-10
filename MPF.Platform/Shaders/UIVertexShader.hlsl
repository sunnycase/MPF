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
extern tagStorkeInfo _stroke : register(c8);
extern float4 _color : register(c9);
extern matrix Model : register(c10);
extern matrix GeometryTransform : register(c14);

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.Position, 1.f);

	pos = mul(pos, Model);
	pos = mul(pos, GeometryTransform);
	pos = mul(pos, _wvp.WorldView);
	int segmentType = input.SegmentType;
	if (segmentType == ST_Linear)
		pos.xy += _stroke.thickness * input.Normal / 2.f;
	pos = mul(pos, _wvp.Projection);

	output.Position = pos;
	output.NormalAndThickness = float3(input.Normal, _stroke.thickness);
	output.Color = _color;
	output.ParamFormValue = input.ParamFormValue;
	output.SegmentType = segmentType;
	return output;
}