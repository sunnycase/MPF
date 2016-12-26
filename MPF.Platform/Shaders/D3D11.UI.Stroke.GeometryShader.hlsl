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

float GetDistance(float2 m, float2 a, float2 b)
{
	float A = b.y - a.y;
	float B = a.x - b.x;
	float C = a.y * b.x - b.y * a.x;
	return abs(A * m.x + B * m.y + C) / sqrt(A * A + B * B);
}

float GetMinComponent(float3 value)
{
	return min(min(value.x, value.y), value.z);
}

float GetMaxComponent(float3 value)
{
	return max(max(value.x, value.y), value.z);
}

[maxvertexcount(3)]
void main(
	triangle PixelShaderInput input[3],
	inout TriangleStream<PixelShaderInput> output
)
{
	int segmentType = input[0].SegmentType;
	if(segmentType == ST_Linear)
	{
		for (uint i = 0; i < 3; i++)
		{
			PixelShaderInput element = input[i];
			element.Position.xy += Thickness / 2.f * element.NormalAndThickness.xy;
			element.Position = mul(element.Position, Projection);
			output.Append(element);
		}
	}
	else if (segmentType == ST_QuadraticBezier)
	{
		float3 A = input[0].Position.xyz;
		float3 B = input[1].Position.xyz;
		float3 C = input[2].Position.xyz;
		float3 M = (A + B + C) / 3.f;

		float minLen = min(min(GetDistance(M, A, B), GetDistance(M, A, C)), GetDistance(M, B, C));
		float scale = Thickness / 1.5f / minLen + 1.f;

		float2 tcA = input[0].ParamFormValue;
		float2 tcB = input[1].ParamFormValue;
		float2 tcC = input[2].ParamFormValue;
		float2 tcM = (tcA + tcB + tcC) / 3.f;

		float halfThickness = Thickness / 2.f;

		float2 APos = input[0].Position.xy;
		float2 BPos = input[1].Position.xy;
		float2 CPos = input[2].Position.xy;

		for (uint i = 0; i < 3; i++)
		{
			PixelShaderInput element = input[i];
			element.Position.xyz = M + (element.Position.xyz - M) * scale;
			element.Position = mul(element.Position, Projection);
			element.ParamFormValue = tcM + (element.ParamFormValue - tcM) * scale;
			element.LeftTopRightBottom = float4(GetMinComponent(float3(APos.x, BPos.x, CPos.x)) - halfThickness, GetMinComponent(float3(APos.y, BPos.y, CPos.y)) - halfThickness,
				GetMaxComponent(float3(APos.x, BPos.x, CPos.x)) + halfThickness, GetMaxComponent(float3(APos.y, BPos.y, CPos.y)) + halfThickness);
			output.Append(element);
		}
	}
	else
	{
		for (uint i = 0; i < 3; i++)
		{
			PixelShaderInput element = input[i];
			element.Position = mul(element.Position, Projection);
			output.Append(element);
		}
	}
}