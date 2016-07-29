#include "UIShaders.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	clip(input.ParamFormCoff.x * pow(input.ParamFormValue.x, 3) + input.ParamFormCoff.y * pow(input.ParamFormValue.x, 2) +
		input.ParamFormCoff.z * input.ParamFormValue.x + input.ParamFormCoff.w * input.ParamFormValue.y);
	return input.Color;
}