#include "D3D11.UIShaders.hlsli"

cbuffer WorldViewProjectionBuffer : register(b0)
{
	matrix WorldView;
	matrix Projection;
};

cbuffer ModelBuffer : register(b1)
{
	matrix Model;
};

cbuffer GeomtryData : register(b2)
{
	matrix GeometryTransform;
	float4 Color;
	float Thickness;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output = (PixelShaderInput)0;
	float4 pos = float4(input.Position, 1.f);

	pos = mul(pos, Model);
	pos = mul(pos, GeometryTransform);
	pos = mul(pos, WorldView);
	
	int segmentType = input.SegmentType;
	if (segmentType == ST_Linear)
	{
		float2 normal = input.Data1;
		normal = mul(normal, (float2x2)Model);
		normal = mul(normal, (float2x2)GeometryTransform);
		normal = mul(normal, (float2x2)WorldView);
		normal = normalize(normal) * length(input.Data1);
		pos.xy += Thickness / 2.f * normal;
		output.NormalAndThickness = float3(normal, Thickness);
		output.ParamFormValue = input.Data2;
	}
	else
	{
		output.NormalAndThickness = float3(0, 0, Thickness);
		if (segmentType == ST_QuadraticBezier)
		{
			float4 mPoint = float4(input.Data1, 0, 1);
			mPoint = mul(mPoint, Model);
			mPoint = mul(mPoint, GeometryTransform);
			mPoint = mul(mPoint, WorldView);

			float minLen = input.Data3.x * 0.1f;
			float scale = Thickness / minLen + 1.f;
			pos = mPoint + (pos - mPoint) * scale;

			float2 mTc = (float2(0, 0) + float2(1, 1) + float2(0.5f, 0)) / 3.f;
			float2 tc = input.Data2;
			tc = mTc + (tc - mTc) * scale;
			output.ParamFormValue = tc;
		}
	}

	pos = mul(pos, Projection);
	output.Position = pos;
	output.Color = Color;
	output.SegmentType = segmentType;
	return output;
}