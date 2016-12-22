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
		output.NormalAndThickness = float3(normal, Thickness);
	}
	else
	{
		output.NormalAndThickness = float3(0, 0, Thickness);
		output.ParamFormValue = input.Data2;
	}

	//pos = mul(pos, Projection);
	output.Position = pos;
	output.Color = Color;
	output.SegmentType = segmentType;
	return output;
}