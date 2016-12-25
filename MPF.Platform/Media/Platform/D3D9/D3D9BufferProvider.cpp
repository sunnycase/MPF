//
// MPF Platform
// Direct3D 9 Buffer Provider
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#include "stdafx.h"
#include "../D3D9PlatformProvider.h"
#include "D3D9DeviceContext.h"
#include <algorithm>
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D9;

template<BufferTypes BufferType>
using BufferProvider = PlatformProvider<PlatformId::D3D9>::BufferProvider<BufferType>;
using DeviceContext = PlatformProvider<PlatformId::D3D9>::DeviceContext;

WRL::ComPtr<IDirect3DVertexBuffer9> BufferProvider<BufferTypes::VertexBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count)
{
	WRL::ComPtr<IDirect3DVertexBuffer9> buffer;
	ThrowIfFailed(deviceContext->Device->CreateVertexBuffer(stride * count, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &buffer, nullptr));
	return buffer;
}

RenderCall PlatformProvider<PlatformId::D3D9>::GetRenderCall(VertexBufferManager<PlatformId::D3D9>& vbMgr, size_t stride, const RentInfo& rent)
{
	return{ &vbMgr, rent.entryIdx, stride, UINT(rent.offset), UINT(rent.length / 3) };
}

void BufferProvider<BufferTypes::VertexBuffer>::Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer)
{
	void* pData = nullptr;
	const auto size = data.size();
	ThrowIfFailed(buffer->Lock(0, size, &pData, D3DLOCK_DISCARD));
	auto fin = make_finalizer([&] {buffer->Unlock(); });
	ThrowIfNot(memcpy_s(pData, size, data.data(), size) == 0, L"Cannot upload vertices.");
}