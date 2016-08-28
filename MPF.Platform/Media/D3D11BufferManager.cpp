//
// MPF Platform
// Direct3D 11 Buffer Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#include "stdafx.h"
#include "D3D11BufferManager.h"
#include <algorithm>
using namespace WRL;
using namespace NS_PLATFORM;

D3D11VertexBufferManager::D3D11VertexBufferManager(ID3D11Device * device, UINT stride)
	:_device(device), _stride(stride)
{
}


RentInfo D3D11VertexBufferManager::Allocate(size_t length)
{
	RentInfo info;
	for (size_t i = 0; i < _buffers.size(); ++i)
	{
		if (_buffers[i].TryAllocate(length, info))
		{
			info.entryIdx = i;
			return info;
		}
	}
	info.entryIdx = _buffers.size();
	_buffers.emplace_back(_device.Get(), _stride, std::max(length, size_t(1024)));
	ThrowIfNot(_buffers.back().TryAllocate(length, info), L"Cannot allocate vertices.");
	return info;
}

void D3D11VertexBufferManager::Retire(const RentInfo & rent)
{
	_buffers[rent.entryIdx].Retire(rent);
}

void D3D11VertexBufferManager::Update(const RentInfo & rent, size_t offset, const void* src, size_t length)
{
	_buffers[rent.entryIdx].Update(rent, offset, src, length);
}

void D3D11VertexBufferManager::Upload(ID3D11DeviceContext * deviceContext)
{
	for (auto&& buffer : _buffers)
		buffer.Upload(deviceContext);
}

D3D11VertexBufferManager::BufferEntry::BufferEntry(ID3D11Device * device, UINT stride, size_t vertexCount)
	:_stride(stride), _freeList(vertexCount)
{
	ThrowIfFailed(device->CreateBuffer(&CD3D11_BUFFER_DESC(vertexCount * stride, D3D11_BIND_VERTEX_BUFFER), nullptr, &_buffer));
	_cpuData.resize(stride * vertexCount);
}

bool D3D11VertexBufferManager::BufferEntry::TryAllocate(size_t length, RentInfo & rent)
{
	size_t offset;
	if (_freeList.TryAllocate(length, offset))
	{
		rent.offset = offset;
		rent.length = length;
		return true;
	}
	return false;
}

void D3D11VertexBufferManager::BufferEntry::Retire(const RentInfo & rent)
{
	_freeList.Retire(rent.offset, rent.length);
}

void D3D11VertexBufferManager::BufferEntry::Update(const RentInfo & rent, size_t offset, const void* src, size_t length)
{
	assert(rent.length >= offset + length);

	auto cpuOffset = rent.offset + offset;
	ThrowIfNot(memcpy_s(&_cpuData.at(cpuOffset * _stride), (_cpuData.size() - cpuOffset * _stride), src, length * _stride) == 0,
		L"Cannot update vertices.");
	_gpuDirty = true;
}

void D3D11VertexBufferManager::BufferEntry::Upload(ID3D11DeviceContext * deviceContext)
{
	if (_gpuDirty)
	{
		deviceContext->UpdateSubresource(_buffer.Get(), 0, nullptr, _cpuData.data(), _cpuData.size(), 0);
		_gpuDirty = false;
	}
}

ID3D11Buffer* D3D11VertexBufferManager::GetVertexBuffer(const RenderCall& renderCall) const noexcept
{
	return _buffers[renderCall.BufferIdx].GetBuffer();
}

RenderCall D3D11VertexBufferManager::GetDrawCall(const RentInfo& rent)
{
	return{ this, rent.entryIdx, _stride, UINT(rent.offset), UINT(rent.length) };
}