//
// MPF Platform
// Buffer Manager
// 作者：SunnyCase
// 创建时间：2016-12-25
//
#pragma once
#include "../inc/common.h"
#include "../inc/FreeList.h"
#include "Platform/PlatformProvider.h"

DEFINE_NS_PLATFORM

template<PlatformId PId, BufferTypes BufferType>
class BufferManager
{
	using PlatformProvider_t = PlatformProvider<PId>;
	using DeviceContext = typename PlatformProvider_t::DeviceContext;
	using BufferProvider = typename PlatformProvider_t::template BufferProvider<BufferType>;
	using NativeType = typename BufferProvider::NativeType;
	using RenderCall = typename PlatformProvider_t::RenderCall;

	class BufferEntry
	{
	public:
		BufferEntry(DeviceContext& deviceContext, size_t stride, size_t count)
			:_stride(stride), _freeList(count),
			_buffer(_bufferProvider.CreateBuffer(deviceContext, stride, count))
		{
			_cpuData.resize(stride * count);
		}

		bool TryAllocate(size_t length, RentInfo& rent)
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

		void Retire(const RentInfo& rent)
		{
			_freeList.Retire(rent.offset, rent.length);
		}

		void Update(const RentInfo& rent, size_t offset, const void* src, size_t length)
		{
			assert(rent.length >= offset + length);

			auto cpuOffset = rent.offset + offset;
			ThrowIfNot(memcpy_s(&_cpuData.at(cpuOffset * _stride), (_cpuData.size() - cpuOffset * _stride), src, length * _stride) == 0,
				L"Cannot update vertices.");
			_gpuDirty = true;
		}

		void Upload(DeviceContext& deviceContext)
		{
			if (_gpuDirty)
			{
				_bufferProvider.Upload(deviceContext, _cpuData, _buffer);
				_gpuDirty = false;
			}
		}

		const NativeType& GetBuffer() const noexcept { return _buffer; }
	private:
		BufferProvider _bufferProvider;
		const size_t _stride;
		NativeType _buffer;
		FreeList _freeList;
		std::vector<byte> _cpuData;
		bool _gpuDirty = false;
	};
public:
	BufferManager(DeviceContext& deviceContext, UINT stride)
		:_deviceContext(deviceContext), _stride(stride)
	{

	}

	const NativeType& GetBuffer(const RenderCall& renderCall) const noexcept
	{
		return _buffers[renderCall.BufferIdx].GetBuffer();
	}

	RentInfo Allocate(size_t length)
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
		_buffers.emplace_back(_deviceContext, _stride, std::max(length, size_t(1024)));
		ThrowIfNot(_buffers.back().TryAllocate(length, info), L"Cannot allocate vertices.");
		return info;
	}

	void Retire(const RentInfo& rent)
	{
		_buffers[rent.entryIdx].Retire(rent);
	}

	void Update(const RentInfo& rent, size_t offset, const void* src, size_t length)
	{
		_buffers[rent.entryIdx].Update(rent, offset, src, length);
	}

	void Upload()
	{
		for (auto&& buffer : _buffers)
			buffer.Upload(_deviceContext);
	}

	RenderCall GetRenderCall(const RentInfo& rent)
	{
		return _platformProvider.GetRenderCall(*this, _stride, rent);
	}
private:
	PlatformProvider_t _platformProvider;
	BufferProvider _bufferProvider;
	const UINT _stride;
	DeviceContext _deviceContext;
	std::vector<BufferEntry> _buffers;
};

END_NS_PLATFORM