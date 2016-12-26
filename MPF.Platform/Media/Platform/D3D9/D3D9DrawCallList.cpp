//
// MPF Platform
// D3D9 Draw Call List
// 作者：SunnyCase
// 创建时间：2016-12-26
//
#include "stdafx.h"
#include "D3D9DeviceContext.h"
#include "../../ResourceManager.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D9;
using namespace DirectX;

void PlatformProvider<PlatformId::D3D9>::PlayRenderCall(const PlayRenderCallArgs<PlatformId::D3D9>& args)
{
	using namespace D3D9;
	auto device = args.DeviceContext->Device;

	ThrowIfFailed(device->SetVertexShaderConstantF(VSCSlot_ModelTransform, args.ModelTransform.m[0], VSCSize_ModelTransform));
	float constants[4] = { 0 };
	for (auto&& rc : args.StrokeRenderCalls)
	{
		auto vb = args.ResMgr->GetVertexBuffer(rc);
		ThrowIfFailed(device->SetStreamSource(0, vb.Get(), 0, rc.Stride));
		constants[0] = rc.Thickness;
		ThrowIfFailed(device->SetVertexShaderConstantF(VSCSlot_Thickness, constants, VSCSize_Thickness));
		ThrowIfFailed(device->SetVertexShaderConstantF(VSCSlot_Color, rc.Color, VSCSize_Color));
		ThrowIfFailed(device->SetVertexShaderConstantF(VSCSlot_GeometryTransform, rc.Transform.m[0], VSCSize_GeometryTransform));
		ThrowIfFailed(device->DrawPrimitive(D3DPT_TRIANGLELIST, rc.StartVertex, rc.PrimitiveCount));
	}
}