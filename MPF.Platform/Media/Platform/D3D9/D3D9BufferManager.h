//
// MPF Platform
// Direct3D 9 Buffer Manager
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#pragma once
#include "../../inc/common.h"
#include <d3d9.h>
#include <list>
#include "../D3D9PlatformProvider.h"

DEFINE_NS_PLATFORM_D3D9

class D3D9VertexBufferManager
{
	class BufferEntry
	{
		struct FreeEntry
		{
			size_t offset;
			size_t length;
		};
	public:
		BufferEntry(IDirect3DDevice9* device, UINT stride, size_t vertexCount = 1024);

		bool TryAllocate(size_t length, RentInfo& rent);
		void Retire(const RentInfo& rent);
		void Update(const RentInfo& rent, size_t offset, const void* src, size_t length);
		void Upload();
		void BeginResetDevice();
		void EndResetDevice(IDirect3DDevice9* device);

		IDirect3DVertexBuffer9* GetBuffer() const noexcept { return _buffer.Get(); }
	private:
		void CombineFreeNode(typename std::list<FreeEntry>::iterator it);
	private:
		const UINT _stride;
		WRL::ComPtr<IDirect3DVertexBuffer9> _buffer;
		std::list<FreeEntry> _freeList;
		std::vector<byte> _cpuData;
		bool _gpuDirty = false;
	};
public:
	D3D9VertexBufferManager(IDirect3DDevice9* device, UINT stride);

	IDirect3DVertexBuffer9* GetVertexBuffer(const RenderCall& renderCall) const noexcept;
	RentInfo Allocate(size_t length);
	void Retire(const RentInfo& rent);
	void Update(const RentInfo& rent, size_t offset, const void* src, size_t length);
	void Upload();
	RenderCall GetDrawCall(const RentInfo& rent);

	void BeginResetDevice();
	void EndResetDevice();
private:
private:
	const UINT _stride;
	WRL::ComPtr<IDirect3DDevice9> _device;
	std::vector<BufferEntry> _buffers;
};

END_NS_PLATFORM_D3D9