#include "D3D11.UIShaders.hlsli"

float4 main(VertexShaderInput input) : SV_POSITION
{
	return float4(input.Position, 1.f);
}