#include "D3D11.UIShaders.hlsli"

struct WorldViewProjection
{
	matrix WorldView;
	matrix Projection;

};
extern WorldViewProjection _wvp : register(b0);
extern matrix Model : register(b1);

float4 main(VertexShaderInput input) : SV_POSITION
{
	PixelShaderInput output;
float4 pos = float4(input.Position, 1.f);

pos = mul(pos, _wvp.WorldView);
//pos = mul(pos, Model);

int segmentType = input.SegmentType;
if (segmentType == ST_Linear)
{
	float2 normal = input.Data1;
	normal = mul(normal, (float2x2)_wvp.WorldView);
	//normal = mul(normal, (float2x2)Model);
	normal = normalize(normal) * length(input.Data1);
	pos.xy += 2 / 2.f * normal;
	output.NormalAndThickness = float3(normal, 2);
	output.ParamFormValue = input.Data2;
}
pos = mul(pos, _wvp.Projection);
output.Position = pos;
output.SegmentType = segmentType;
return output.Position;
}