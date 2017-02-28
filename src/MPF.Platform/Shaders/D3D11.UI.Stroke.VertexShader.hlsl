#include "D3D11.UI.Stroke.Shaders.hlsli"

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

	pos = mul(pos, GeometryTransform);
	pos = mul(pos, Model);
	pos = mul(pos, WorldView);
	
	int segmentType = input.SegmentType;
	if (segmentType == ST_Linear)
	{
		float2 normal = input.ParamFormValue;
		normal = mul(normal, (float2x2)GeometryTransform);
		normal = mul(normal, (float2x2)Model);
		normal = mul(normal, (float2x2)WorldView);
		normal = normalize(normal) * length(input.ParamFormValue);

		output.ParamFormValue = normal;
	}
	else
		output.ParamFormValue = input.ParamFormValue;

	output.Thickness = Thickness;
	output.Position = pos;
	output.Color = Color;
	output.SegmentType = segmentType;
	return output;
}