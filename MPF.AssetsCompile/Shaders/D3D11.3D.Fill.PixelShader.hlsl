#include "D3D11.3D.Fill.Shaders.hlsli"

Texture2D Tex1;
SamplerState Sampler1;

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(1, 0, 0, 1) + Tex1.Sample(Sampler1, input.TexCoord.xy);
}