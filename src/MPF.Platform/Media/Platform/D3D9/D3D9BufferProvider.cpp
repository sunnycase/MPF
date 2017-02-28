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

void PlatformProvider<PlatformId::D3D9>::GetRenderCall(RenderCall& rc, VertexBufferManager<PlatformId::D3D9>& vbMgr, size_t stride, const BufferRentInfo& rent)
{
	rc.VB.Mgr = &vbMgr;
	rc.VB.BufferIdx = rent.entryIdx;
	rc.VB.Stride = stride;
	rc.VB.Start = rent.offset;
	rc.VB.Count = rent.length;
}

void BufferProvider<BufferTypes::VertexBuffer>::Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer)
{
	void* pData = nullptr;
	const auto size = data.size();
	ThrowIfFailed(buffer->Lock(0, size, &pData, D3DLOCK_DISCARD));
	auto fin = make_finalizer([&] {buffer->Unlock(); });
	ThrowIfNot(memcpy_s(pData, size, data.data(), size) == 0, L"Cannot upload vertices.");
}

namespace
{
	D3DFORMAT StrideToIndexFormat(size_t stride)
	{
		switch (stride)
		{
		case 2:
			return D3DFMT_INDEX16;
		case 4:
			return D3DFMT_INDEX32;
		default:
			ThrowIfFailed(E_INVALIDARG);
		}
	}
}


WRL::ComPtr<IDirect3DIndexBuffer9> BufferProvider<BufferTypes::IndexBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count)
{
	WRL::ComPtr<IDirect3DIndexBuffer9> buffer;
	ThrowIfFailed(deviceContext->Device->CreateIndexBuffer(stride * count, D3DUSAGE_WRITEONLY, StrideToIndexFormat(stride), D3DPOOL_DEFAULT, &buffer, nullptr));
	return buffer;
}

void PlatformProvider<PlatformId::D3D9>::GetRenderCall(RenderCall& rc, IndexBufferManager<PlatformId::D3D9>& ibMgr, size_t stride, const BufferRentInfo& rent)
{
	rc.IB.Mgr = &ibMgr;
	rc.IB.BufferIdx = rent.entryIdx;
	rc.IB.Start = rent.offset;
	rc.PrimitiveCount = rent.length / 3;
}

void BufferProvider<BufferTypes::IndexBuffer>::Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer)
{
	void* pData = nullptr;
	const auto size = data.size();
	ThrowIfFailed(buffer->Lock(0, size, &pData, D3DLOCK_DISCARD));
	auto fin = make_finalizer([&] {buffer->Unlock(); });
	ThrowIfNot(memcpy_s(pData, size, data.data(), size) == 0, L"Cannot upload indices.");
}

auto BufferProvider<BufferTypes::TextureBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t count) -> NativeType
{
	return std::vector<WRL::ComPtr<IDirect3DTexture9>>(count);
}

void BufferProvider<BufferTypes::TextureBuffer>::Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data)
{
	for (auto& item : data)
		ThrowIfFailed(deviceContext->Device->CreateTexture(item.Width, item.Height, item.Levels, item.Usage,
			item.Format, D3DPOOL_DEFAULT, &buffer.at(offset++), nullptr));
}

void BufferProvider<BufferTypes::TextureBuffer>::Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data)
{
	for (auto& item : data)
	{
		auto tex = buffer.at(offset);
		ThrowIfFailed(E_NOTIMPL);
	}
}

void BufferProvider<BufferTypes::TextureBuffer>::Retire(NativeType& buffer, size_t offset, size_t length)
{
	for (size_t i = 0; i < length; i++)
		buffer.at(offset + i).Reset();
}

auto BufferProvider<BufferTypes::SamplerBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t count) -> NativeType
{
	return std::vector<SamplerStates>(count);
}

void BufferProvider<BufferTypes::SamplerBuffer>::Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data)
{
	for (auto& item : data)
		buffer.at(offset++) = item;
}

auto BufferProvider<BufferTypes::ShaderBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t count) -> NativeType
{
	return std::vector<ShaderEntry>(count);
}

void BufferProvider<BufferTypes::ShaderBuffer>::Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data)
{
	for (auto& item : data)
	{
		if (!item.VertexShader.empty())
			ThrowIfFailed(deviceContext->Device->CreateVertexShader(reinterpret_cast<const DWORD*>(item.VertexShader.data()),
				&buffer.at(offset).VertexShader));
		if (!item.PixelShader.empty())
			ThrowIfFailed(deviceContext->Device->CreatePixelShader(reinterpret_cast<const DWORD*>(item.PixelShader.data()),
				&buffer.at(offset).PixelShader));
	}
}

void BufferProvider<BufferTypes::ShaderBuffer>::Retire(NativeType& buffer, size_t offset, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		auto& entry = buffer.at(offset + i);
		entry.VertexShader.Reset();
		entry.PixelShader.Reset();
	}
}