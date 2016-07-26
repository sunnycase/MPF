#include "UIShaders.hlsli"

struct tagWorldViewProj
{
	matrix worldMat;
	matrix viewMat;
	matrix projMat;
};
extern tagWorldViewProj WVP: register(c0);

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.Position, 1.f);
	pos = mul(pos, WVP.worldMat);
	pos = mul(pos, WVP.viewMat);
	pos = mul(pos, WVP.projMat);

	output.Position = pos;
	output.Color = input.Color;
	return output;
}