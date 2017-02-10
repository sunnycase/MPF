//
// MPF Platform
// D3D11 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#include "stdafx.h"
#include "D3D11DeviceContext.h"
#include "../../ResourceManager.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D11;
using namespace DirectX;

void PlatformProvider<PlatformId::D3D11>::PlayRenderCall(const PlayRenderCallArgs<PlatformId::D3D11>& args)
{
	using namespace D3D11;

	auto& updateContext = args.DeviceContext->UpdateContext;
	auto deviceContext = args.DeviceContext->DeviceContext;
	{
		auto model = updateContext.Model.Map(deviceContext);
		model->Model = args.ModelTransform;
	}

	UINT offset = 0;
	// Fill
	args.DeviceContext->SetPipelineState(PiplineStateTypes::Fill);
	for (auto&& rc : args.FillRenderCalls)
	{
		auto vb = args.ResMgr->GetVertexBuffer(rc);
		auto ib = args.ResMgr->GetIndexBuffer(rc);
		deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &rc.VB.Stride, &offset);
		deviceContext->IASetIndexBuffer(ib.Get(), rc.IB.Format, 0);

		{
			auto geo = updateContext.Geometry.Map(deviceContext);
			ThrowIfNot(memcpy_s(geo->Color, sizeof(geo->Color), rc.Color, sizeof(rc.Color)) == 0, L"Cannot copy color.");
			geo->Transform = rc.Transform;
		}
		deviceContext->DrawIndexed(rc.IB.Count, rc.IB.Start, rc.VB.Start);
	}

	// Stroke
	args.DeviceContext->SetPipelineState(PiplineStateTypes::Stroke);
	for (auto&& rc : args.StrokeRenderCalls)
	{
		auto vb = args.ResMgr->GetVertexBuffer(rc);
		auto ib = args.ResMgr->GetIndexBuffer(rc);
		deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &rc.VB.Stride, &offset);
		deviceContext->IASetIndexBuffer(ib.Get(), rc.IB.Format, 0);

		{
			auto geo = updateContext.Geometry.Map(deviceContext);
			ThrowIfNot(memcpy_s(geo->Color, sizeof(geo->Color), rc.Color, sizeof(rc.Color)) == 0, L"Cannot copy color.");
			geo->Thickness = rc.Thickness;
			geo->Transform = rc.Transform;
		}
		deviceContext->DrawIndexed(rc.IB.Count, rc.IB.Start, rc.VB.Start);
	}
}
