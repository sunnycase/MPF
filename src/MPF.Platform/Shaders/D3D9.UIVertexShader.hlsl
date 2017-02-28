#include "D3D9.UIShaders.hlsli"

struct WorldViewProjection
{
	matrix WorldView;
	matrix Projection;
};
extern WorldViewProjection _wvp: register(c0);

struct tagStrokeInfo
{
	float thickness;
	float padding[3];
};
extern tagStrokeInfo _stroke : register(c8);
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
	{
		float2 normal = input.Data1;
		normal = mul(normal, (float2x2)Model);
		normal = mul(normal, (float2x2)GeometryTransform);
		normal = mul(normal, (float2x2)_wvp.WorldView);
		normal = normalize(normal) * length(input.Data1);
		pos.xy += _stroke.thickness / 2.f * normal;
		output.NormalAndThickness = float3(normal, _stroke.thickness);
		output.ParamFormValue = input.Data2;
	}
	else
	{
		output.NormalAndThickness = float3(0, 0, _stroke.thickness);
		if (segmentType == ST_QuadraticBezier)
		{
			float4 mPoint = float4(input.Data1, 0, 1);
			mPoint = mul(mPoint, Model);
			mPoint = mul(mPoint, GeometryTransform);
			mPoint = mul(mPoint, _wvp.WorldView);

			float minLen = input.Data3.x * 0.1f;

			float scale = _stroke.thickness / minLen + 1.f;
			pos = mPoint + (pos - mPoint) * scale;

			float2 mTc = (float2(0, 0) + float2(1, 1) + float2(0.5f, 0)) / 3.f;
			float2 tc = input.Data2;
			tc = mTc + (tc - mTc) * scale;
			output.ParamFormValue = tc;
		}
	}

	pos = mul(pos, _wvp.Projection);
	output.Position = pos;
	output.Color = _color;
	output.SegmentType = segmentType;
	return output;
}