#include "D3D11.3D.Fill.Shaders.hlsli"

Texture2D Tex1;
sampler Sampler1;

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(1, 0, 0, 1);
}