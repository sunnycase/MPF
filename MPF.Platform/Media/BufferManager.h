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
#include <limits>
#include <concurrent_queue.h>

DEFINE_NS_PLATFORM

namespace Details
{
	template<PlatformId PId, BufferTypes BufferType>
	class SimpleBufferEntry
	{
	public:
		using PlatformProvider_t = PlatformProvider<PId>;
		using DeviceContext = typename PlatformProvider_t::DeviceContext;
		using BufferProvider = typename PlatformProvider_t::template BufferProvider<BufferType>;
		using NativeType = typename BufferProvider::NativeType;
		using RenderCall = typename PlatformProvider_t::RenderCall;

		SimpleBufferEntry(DeviceContext& deviceContext, size_t stride, size_t count)
			:_stride(stride), _freeList(count),
			_buffer(_bufferProvider.CreateBuffer(deviceContext, stride, count))
		{
			_cpuData.resize(stride * count);
		}

		bool TryAllocate(size_t length, BufferRentInfo& rent)
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

		void Retire(const BufferRentInfo& rent)
		{
			_freeList.Retire(rent.offset, rent.length);
		}

		void Update(const BufferRentInfo& rent, size_t offset, const void* src, size_t length)
		{
			assert(rent.length >= offset + length);

			auto cpuOffset = rent.offset + offset;
			ThrowIfNot(memcpy_s(&_cpuData.at(cpuOffset * _stride), (_cpuData.size() - cpuOffset * _stride), src, length * _stride) == 0,
				L"Cannot update buffer data.");
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
		size_t GetStride() const noexcept { return _stride; }
	private:
		BufferProvider _bufferProvider;
		const size_t _stride;
		NativeType _buffer;
		FreeList _freeList;
		std::vector<byte> _cpuData;
		bool _gpuDirty = false;
	};

	template<PlatformId PId, BufferTypes BufferType, template<PlatformId, BufferTypes> class BufferEntry>
	class BufferManagerImpl
	{
	protected:
		using PlatformProvider_t = PlatformProvider<PId>;
		using DeviceContext = typename PlatformProvider_t::DeviceContext;
		using BufferProvider = typename PlatformProvider_t::template BufferProvider<BufferType>;
		using NativeType = typename BufferProvider::NativeType;
		using RenderCall = typename PlatformProvider_t::RenderCall;
	public:
		BufferManagerImpl(DeviceContext& deviceContext)
			:_deviceContext(deviceContext), _buffersLock(500)
		{

		}

		template<typename T>
		const NativeType& GetBuffer(const T& renderCall) const noexcept
		{
			auto locker = _buffersLock.Lock();
			return _buffers[renderCall.BufferIdx].GetBuffer();
		}

		void Retire(const BufferRentInfo& rent)
		{
			auto locker = _buffersLock.Lock();
			_buffers[rent.entryIdx].Retire(rent);
		}

		void Upload()
		{
			auto locker = _buffersLock.Lock();
			for (auto&& buffer : _buffers)
				buffer.Upload(_deviceContext);
		}
	protected:
		BufferRentInfo AllocateCore(size_t length, size_t stride)
		{
			auto locker = _buffersLock.Lock();
			BufferRentInfo info;
			for (size_t i = 0; i < _buffers.size(); ++i)
			{
				if (_buffers[i].TryAllocate(length, info))
				{
					info.entryIdx = i;
					return info;
				}
			}
			info.entryIdx = _buffers.size();
			_buffers.emplace_back(_deviceContext, stride, std::max(length, size_t(1024)));
			ThrowIfNot(_buffers.back().TryAllocate(length, info), L"Cannot allocate buffer data.");
			return info;
		}
	protected:
		PlatformProvider_t _platformProvider;
		BufferProvider _bufferProvider;
		DeviceContext _deviceContext;
		mutable WRL::Wrappers::CriticalSection _buffersLock;
		std::vector<BufferEntry<PId, BufferType>> _buffers;
	};

	template<PlatformId PId, BufferTypes BufferType>
	class BufferEntry
	{
	public:
		using PlatformProvider_t = PlatformProvider<PId>;
		using DeviceContext = typename PlatformProvider_t::DeviceContext;
		using BufferProvider = typename PlatformProvider_t::template BufferProvider<BufferType>;
		using NativeType = typename BufferProvider::NativeType;
		using RenderCall = typename PlatformProvider_t::RenderCall;
		using RentUpdateContext = typename BufferProvider::RentUpdateContext;

		BufferEntry(DeviceContext& deviceContext, size_t stride, size_t count)
			:_freeList(count),
			_buffer(_bufferProvider.CreateBuffer(deviceContext, count))
		{

		}

		bool TryAllocate(size_t length, BufferRentInfo& rent)
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

		void Retire(const BufferRentInfo& rent)
		{
			_bufferProvider.Retire(_buffer, rent.offset, rent.length);
			_freeList.Retire(rent.offset, rent.length);
		}

		void Update(DeviceContext& deviceContext, const BufferRentInfo& rent, size_t offset, std::vector<RentUpdateContext>&& src)
		{
			assert(rent.length >= offset + src.size());

			auto cpuOffset = rent.offset + offset;
			_bufferProvider.Update(deviceContext, _buffer, cpuOffset, src);
			_itemsToUpload.push({ cpuOffset, std::move(src) });
		}

		void Upload(DeviceContext& deviceContext)
		{
			if (!_itemsToUpload.empty())
			{
				std::pair<size_t, std::vector<RentUpdateContext>> itemToUpload;
				while(_itemsToUpload.try_pop(itemToUpload))
					_bufferProvider.Upload(deviceContext, _buffer, itemToUpload.first, std::move(itemToUpload.second));
			}
		}

		const NativeType& GetBuffer() const noexcept { return _buffer; }
	private:
		BufferProvider _bufferProvider;
		DeviceContext _deviceContext;
		NativeType _buffer;
		FreeList _freeList;
		concurrency::concurrent_queue<std::pair<size_t, std::vector<RentUpdateContext>>> _itemsToUpload;
	};
}

template<PlatformId PId, BufferTypes BufferType>
class BufferManager;

template<PlatformId PId>
class BufferManager<PId, BufferTypes::VertexBuffer> : public Details::BufferManagerImpl<PId, BufferTypes::VertexBuffer, Details::SimpleBufferEntry>
{
public:
	BufferManager(DeviceContext& deviceContext, UINT stride)
		:BufferManagerImpl(deviceContext), _stride(stride)
	{

	}

	BufferRentInfo Allocate(size_t length)
	{
		return AllocateCore(length, _stride);
	}

	void Update(const BufferRentInfo& rent, size_t offset, const void* src, size_t length)
	{
		auto locker = _buffersLock.Lock();
		_buffers[rent.entryIdx].Update(rent, offset, src, length);
	}

	void GetRenderCall(const BufferRentInfo& rent, RenderCall& rc)
	{
		_platformProvider.GetRenderCall(rc, *this, _stride, rent);
	}
private:
	const UINT _stride;
};

template<PlatformId PId>
class BufferManager<PId, BufferTypes::IndexBuffer> : public Details::BufferManagerImpl<PId, BufferTypes::IndexBuffer, Details::SimpleBufferEntry>
{
public:
	BufferManager(DeviceContext& deviceContext)
		:BufferManagerImpl(deviceContext)
	{

	}

	BufferRentInfo Allocate(size_t length)
	{
		return AllocateCore(length, length > size_t(std::numeric_limits<uint16_t>::max()) ? sizeof(uint32_t) : sizeof(uint16_t));
	}

	void Update(const BufferRentInfo& rent, size_t offset, const size_t* src, size_t length)
	{
		auto locker = _buffersLock.Lock();

		auto& buffer = _buffers[rent.entryIdx];
		const auto stride = buffer.GetStride();
		if(stride == sizeof(*src))
			_buffers[rent.entryIdx].Update(rent, offset, src, length);
		else
		{
			static std::vector<uint16_t> tData16;
			static std::vector<uint32_t> tData32;

			void* tDataPtr = nullptr;
			if (stride == sizeof(uint16_t))
			{
				tData16.resize(length);
				for (size_t i = 0; i < length; i++)
					tData16[i] = uint16_t(src[i]);
				tDataPtr = tData16.data();
			}
			else if (stride == sizeof(uint32_t))
			{
				tData32.resize(length);
				for (size_t i = 0; i < length; i++)
					tData32[i] = uint32_t(src[i]);
				tDataPtr = tData32.data();
			}
			else
				ThrowAlways(L"Cannot update index buffer: stride is invalid.");
			_buffers[rent.entryIdx].Update(rent, offset, tDataPtr, length);
		}
	}

	void GetRenderCall(const BufferRentInfo& rent, RenderCall& rc)
	{
		auto locker = _buffersLock.Lock();
		return _platformProvider.GetRenderCall(rc, *this, _buffers[rent.entryIdx].GetStride(), rent);
	}
};

template<PlatformId PId>
class BufferManager<PId, BufferTypes::TextureBuffer> : public Details::BufferManagerImpl<PId, BufferTypes::TextureBuffer, Details::BufferEntry>
{
public:
	using BrushRenderCall = typename PlatformProvider_t::BrushRenderCall;
	using RentUpdateContext = typename BufferProvider::RentUpdateContext;

	BufferManager(DeviceContext& deviceContext)
		:BufferManagerImpl(deviceContext)
	{

	}

	BufferRentInfo Allocate(size_t length)
	{
		return AllocateCore(length, 0);
	}

	void Update(const BufferRentInfo& rent, size_t offset, std::vector<RentUpdateContext>&& src)
	{
		auto locker = _buffersLock.Lock();
		_buffers[rent.entryIdx].Update(_deviceContext, rent, offset, std::move(src));
	}

	void GetRenderCall(const BufferRentInfo& rent, BrushRenderCall& rc)
	{
		_platformProvider.GetRenderCall(rc, *this, rent);
	}
};

template<PlatformId PId>
class BufferManager<PId, BufferTypes::SamplerBuffer> : public Details::BufferManagerImpl<PId, BufferTypes::SamplerBuffer, Details::BufferEntry>
{
public:
	using BrushRenderCall = typename PlatformProvider_t::BrushRenderCall;
	using RentUpdateContext = typename BufferProvider::RentUpdateContext;

	BufferManager(DeviceContext& deviceContext)
		:BufferManagerImpl(deviceContext)
	{

	}

	BufferRentInfo Allocate(size_t length)
	{
		return AllocateCore(length, 0);
	}

	void Update(const BufferRentInfo& rent, size_t offset, std::vector<RentUpdateContext>&& src)
	{
		auto locker = _buffersLock.Lock();
		_buffers[rent.entryIdx].Update(_deviceContext, rent, offset, std::move(src));
	}

	void GetRenderCall(const BufferRentInfo& rent, BrushRenderCall& rc)
	{
		_platformProvider.GetRenderCall(rc, *this, rent);
	}
};

END_NS_PLATFORM