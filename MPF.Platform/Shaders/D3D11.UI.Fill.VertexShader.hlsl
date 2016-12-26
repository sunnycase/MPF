#include "D3D11.UI.Fill.Shaders.hlsli"

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

	pos = mul(pos, Projection);
	output.Position = pos;
	output.Color = Color;
	return output;
}