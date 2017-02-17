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

auto BufferProvider<BufferTypes::TextureBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t count) -> NativeType
{
	return std::vector<TextureEntry>(count);
}

namespace
{

}

void BufferProvider<BufferTypes::TextureBuffer>::Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data)
{
	for (auto& item : data)
	{
		ComPtr<ID3D11Resource> resource;
		switch (item.Dimension)
		{
		case 2:
		{
			ComPtr<ID3D11Texture2D> texture;
			ThrowIfFailed(deviceContext->Device->CreateTexture2D(&item.Desc.Tex2D, nullptr, &texture));
			resource = std::move(texture);
			break;
		}
		default:
			ThrowAlways(L"This texture dimension is not supported.");
			break;
		}
		
		for (size_t i = 0; i < std::size(item.SRVDesc); i++)
		{
			const auto& desc = item.SRVDesc[i];
			if (desc.ViewDimension)
				ThrowIfFailed(deviceContext->Device->CreateShaderResourceView(resource.Get(), &desc, &buffer.at(offset).SRVs[i]));
		}
		buffer.at(offset++).Texture = std::move(resource);
	}
}

void BufferProvider<BufferTypes::TextureBuffer>::Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data)
{
	for (auto& item : data)
	{
		auto entry = buffer.at(offset);
		deviceContext->DeviceContext->UpdateSubresource(entry.Texture.Get(), 0, nullptr, item.Data.data(), item.SrcRowPitch, item.SrcDepthPitch);
	}
}

void BufferProvider<BufferTypes::TextureBuffer>::Retire(NativeType& buffer, size_t offset, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		auto& entry = buffer.at(offset + i);
		entry.Texture.Reset();
		for (auto&& srv : entry.SRVs)
			srv.Reset();
	}
}

auto BufferProvider<BufferTypes::SamplerBuffer>::CreateBuffer(DeviceContext& deviceContext, size_t count) -> NativeType
{
	return std::vector<WRL::ComPtr<ID3D11SamplerState>>(count);
}

void BufferProvider<BufferTypes::SamplerBuffer>::Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data)
{
	for (auto& item : data)
		ThrowIfFailed(deviceContext->Device->CreateSamplerState(&item.Desc, &buffer.at(offset++)));
}

void BufferProvider<BufferTypes::SamplerBuffer>::Retire(NativeType& buffer, size_t offset, size_t length)
{
	for (size_t i = 0; i < length; i++)
		buffer.at(offset + i).Reset();
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
			ThrowIfFailed(deviceContext->Device->CreateVertexShader(item.VertexShader.data(), item.VertexShader.size(), nullptr, &buffer.at(offset).VertexShader));
		if (!item.PixelShader.empty())
			ThrowIfFailed(deviceContext->Device->CreatePixelShader(item.PixelShader.data(), item.PixelShader.size(), nullptr, &buffer.at(offset++).PixelShader));
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

void PlatformProvider<PlatformId::D3D11>::GetRenderCall(BrushRenderCall& rc, BufferManager<PlatformId::D3D11, BufferTypes::TextureBuffer>& tbMgr, const BufferRentInfo& rent)
{
	rc.Texture.Mgr = &tbMgr;
	rc.Texture.BufferIdx = rent.entryIdx;
	rc.Texture.Start = rent.offset;
	rc.Texture.Count = rent.length;
}

void PlatformProvider<PlatformId::D3D11>::GetRenderCall(BrushRenderCall& rc, BufferManager<PlatformId::D3D11, BufferTypes::SamplerBuffer>& sbMgr, const BufferRentInfo& rent)
{
	rc.Sampler.Mgr = &sbMgr;
	rc.Sampler.BufferIdx = rent.entryIdx;
	rc.Sampler.Start = rent.offset;
	rc.Sampler.Count = rent.length;
}

void PlatformProvider<PlatformId::D3D11>::GetRenderCall(MaterialRenderCall& rc, BufferManager<PlatformId::D3D11, BufferTypes::ShaderBuffer>& sbMgr, const BufferRentInfo& rent)
{
	rc.Shader.Mgr = &sbMgr;
	rc.Shader.BufferIdx = rent.entryIdx;
	rc.Shader.Start = rent.offset;
	rc.Shader.Count = rent.length;
}

void PlatformProvider<PlatformId::D3D11>::ConvertRenderCall(const PenRenderCall& prc, StrokeRenderCall<RenderCall>& rc) const
{
	rc.Material.Brush = prc.Brush;
	rc.Thickness = prc.Thickness;
}

void PlatformProvider<PlatformId::D3D11>::ConvertRenderCall(const BrushRenderCall& brc, FillRenderCall<RenderCall>& rc) const
{
	rc.Material.Brush = brc;
}

void PlatformProvider<PlatformId::D3D11>::ConvertRenderCall(const MaterialRenderCall& mrc, Fill3DRenderCall<RenderCall>& rc) const
{
	rc.Material.Material = mrc;
}