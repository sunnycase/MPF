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

RenderCall PlatformProvider<PlatformId::D3D11>::GetRenderCall(VertexBufferManager<PlatformId::D3D11>& vbMgr, size_t stride, const RentInfo& rent)
{
	return{ &vbMgr, rent.entryIdx, stride, UINT(rent.offset), UINT(rent.length) };
}

void BufferProvider<BufferTypes::VertexBuffer>::Upload(DeviceContext& deviceContext, const std::vector<byte>& data, WRL::ComPtr<ID3D11Buffer>& buffer)
{
	deviceContext->DeviceContext->UpdateSubresource(buffer.Get(), 0, nullptr, data.data(), data.size(), 0);
}