#include "UIShaders.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	return input.Color;
}