#include "D3D11.UI.Fill.Shaders.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	return input.Color;
}