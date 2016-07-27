//
// MPF Platform
// Direct3D 9 Buffer Manager
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#include "stdafx.h"
#include "D3D9BufferManager.h"
#include <algorithm>
using namespace WRL;
using namespace NS_PLATFORM;
using namespace D3D;

D3D9VertexBufferManager::D3D9VertexBufferManager(IDirect3DDevice9* device, UINT stride)
	:_device(device), _stride(stride)
{

}

RentInfo D3D9VertexBufferManager::Allocate(size_t length)
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

void D3D9VertexBufferManager::Retire(const RentInfo & rent)
{
	_buffers[rent.entryIdx].Retire(rent);
}

void D3D9VertexBufferManager::Update(const RentInfo & rent, size_t offset, const void* src, size_t length)
{
	_buffers[rent.entryIdx].Update(rent, offset, src, length);
}

void D3D9VertexBufferManager::Upload()
{
	for (auto&& buffer : _buffers)
		buffer.Upload();
}

D3D9VertexBufferManager::BufferEntry::BufferEntry(IDirect3DDevice9 * device, UINT stride, size_t vertexCount)
	:_stride(stride)
{
	ThrowIfFailed(device->CreateVertexBuffer(stride * vertexCount, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &_buffer, nullptr));
	_cpuData.resize(stride * vertexCount);
	_freeList.emplace_back<FreeEntry>({ 0, vertexCount });
}

bool D3D9VertexBufferManager::BufferEntry::TryAllocate(size_t length, RentInfo & rent)
{
	for (auto it = _freeList.begin(); it != _freeList.end(); ++it)
	{
		if (it->length >= length)
		{
			rent.offset = it->offset;
			rent.length = length;
			if (it->length == length)
				it = _freeList.erase(it);
			else
			{
				it->offset += length;
				it->length -= length;
			}
			return true;
		}
	}
	return false;
}

void D3D9VertexBufferManager::BufferEntry::Retire(const RentInfo & rent)
{
	if (_freeList.empty())
		_freeList.emplace_front<FreeEntry>({ rent.offset, rent.length });
	else
	{
		auto it = std::find_if(_freeList.begin(), _freeList.end(), [&](const FreeEntry& entry) {return entry.offset > rent.offset; });
		CombineFreeNode(_freeList.insert(it, { rent.offset, rent.length }));
	}
}

void D3D9VertexBufferManager::BufferEntry::Update(const RentInfo & rent, size_t offset, const void* src, size_t length)
{
	assert(rent.length >= offset + length);

	auto cpuOffset = rent.offset + offset;
	ThrowIfNot(memcpy_s(&_cpuData.at(cpuOffset * _stride), (_cpuData.size() - cpuOffset * _stride), src, length * _stride) == 0,
		L"Cannot update vertices.");
	_gpuDirty = true;
}

void D3D9VertexBufferManager::BufferEntry::Upload()
{
	if (_gpuDirty)
	{
		void* pData = nullptr;
		const auto size = _cpuData.size();
		ThrowIfFailed(_buffer->Lock(0, size, &pData, D3DLOCK_DISCARD));
		auto fin = make_finalizer([&] {_buffer->Unlock(); });
		ThrowIfNot(memcpy_s(pData, size, _cpuData.data(), size) == 0, L"Cannot upload vertices.");
		_gpuDirty = false;
	}
}

void D3D9VertexBufferManager::BufferEntry::CombineFreeNode(typename std::list<FreeEntry>::iterator it)
{
	assert(it != _freeList.end());
	if (it != _freeList.begin())
	{
		auto left = std::prev(it);
		if (left->offset + left->length == it->offset)
		{
			left->length += it->length;
			CombineFreeNode(_freeList.erase(it));
			return;
		}
	}
	auto right = std::next(it);
	if (right != _freeList.end())
	{
		if (it->offset + it->length == right->offset)
		{
			it->length += right->length;
			_freeList.erase(right);
		}
	}
}

RenderCall D3D9VertexBufferManager::GetDrawCall(const RentInfo& rent)
{
	return{ _buffers[rent.entryIdx].GetBuffer(), _stride, UINT(rent.offset), UINT(rent.length / 3) };
}