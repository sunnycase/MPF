#include "UIShaders.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	float thickness = input.NormalAndThickness.z;
	if (input.ParamFormCoff.y)
	{
		float2 px = ddx(input.ParamFormValue);
		float2 py = ddy(input.ParamFormValue);
		// Chain rule  
		float fx = (2 * input.ParamFormValue.x)*px.x - px.y;
		float fy = (2 * input.ParamFormValue.x)*py.x - py.y;
		// Signed distance  
		float sd = (input.ParamFormValue.x * input.ParamFormValue.x - input.ParamFormValue.y) / sqrt(fx*fx + fy*fy);

		clip(thickness / 2.f - abs(sd));
	}
	return input.Color;
}