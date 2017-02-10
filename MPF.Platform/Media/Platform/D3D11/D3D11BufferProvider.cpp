//
// MPF Platform
// Direct3D 11 Buffer Provider
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#include "stdafx.h"
#include "../D3D11PlatformProvider.h"
#include "D3D11DeviceContext.h"
#include <algorithm>
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D11;

template<BufferTypes BufferType>
using BufferProvider = PlatformProvider<PlatformId::D3D11>::BufferProvider<BufferType>;
using DeviceContext = PlatformProvider<PlatformId::D3D11>::DeviceContext;

WRL::ComPtr<ID3D11Buffer> BufferProvider<BufferTypes::VertexBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count)
{
	WRL::ComPtr<ID3D11Buffer> buffer;
	ThrowIfFailed(deviceContext->Device->CreateBuffer(&CD3D11_BUFFER_DESC(count * stride, D3D11_BIND_VERTEX_BUFFER), nullptr, &buffer));
	return buffer;
}

void PlatformProvider<PlatformId::D3D11>::GetRenderCall(RenderCall& rc, VertexBufferManager<PlatformId::D3D11>& vbMgr, size_t stride, const BufferRentInfo& rent)
{
	rc.VB.Mgr = &vbMgr;
	rc.VB.BufferIdx = rent.entryIdx;
	rc.VB.Stride = stride;
	rc.VB.Start = rent.offset;
	rc.VB.Count = rent.length;
}

void BufferProvider<BufferTypes::VertexBuffer>::Upload(DeviceContext& deviceContext, const std::vector<byte>& data, WRL::ComPtr<ID3D11Buffer>& buffer)
{
	deviceContext->DeviceContext->UpdateSubresource(buffer.Get(), 0, nullptr, data.data(), data.size(), 0);
}


WRL::ComPtr<ID3D11Buffer> BufferProvider<BufferTypes::IndexBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count)
{
	WRL::ComPtr<ID3D11Buffer> buffer;
	ThrowIfFailed(deviceContext->Device->CreateBuffer(&CD3D11_BUFFER_DESC(count * stride, D3D11_BIND_INDEX_BUFFER), nullptr, &buffer));
	return buffer;
}

namespace
{
	DXGI_FORMAT StrideToIndexFormat(size_t stride)
	{
		switch (stride)
		{
		case 2:
			return DXGI_FORMAT_R16_UINT;
		case 4:
			return DXGI_FORMAT_R32_UINT;
		default:
			ThrowIfFailed(E_INVALIDARG);
		}
	}
}

void PlatformProvider<PlatformId::D3D11>::GetRenderCall(RenderCall& rc, IndexBufferManager<PlatformId::D3D11>& ibMgr, size_t stride, const BufferRentInfo& rent)
{
	rc.IB.Mgr = &ibMgr;
	rc.IB.BufferIdx = rent.entryIdx;
	rc.IB.Start = rent.offset;
	rc.IB.Format = StrideToIndexFormat(stride);
	rc.IB.Count = rent.length;
}

void BufferProvider<BufferTypes::IndexBuffer>::Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer)
{
	deviceContext->DeviceContext->UpdateSubresource(buffer.Get(), 0, nullptr, data.data(), data.size(), 0);
}