//
// MPF Platform
// Direct3D 9 Buffer Manager
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#pragma once
#include "../../inc/common.h"
#include <d3d9.h>
#include "D3D9Vertex.h"
#include <list>

DEFINE_NS_PLATFORM

template<typename T>
class D3D9BufferManagerBase
{
public:

};

struct RentInfo
{
	size_t entryIdx;
	size_t offset;
	size_t length;
};

struct RenderCall
{
	WRL::ComPtr<IDirect3DVertexBuffer9> VB;
	UINT StartVertex;
	UINT VertexCount;
};

class D3D9VertexBufferManager : public D3D9BufferManagerBase<D3D::Vertex>
{
	class BufferEntry
	{
		struct FreeEntry
		{
			size_t offset;
			size_t length;
		};
	public:
		BufferEntry(IDirect3DDevice9* device, size_t vertexCount = 1024);

		bool TryAllocate(size_t length, RentInfo& rent);
		void Retire(const RentInfo& rent);
		void Update(const RentInfo& rent, size_t offset, const D3D::Vertex* src, size_t count);
		void Upload();

		IDirect3DVertexBuffer9* GetBuffer() const noexcept { return _buffer.Get(); }
	private:
		void CombineFreeNode(typename std::list<FreeEntry>::iterator it);
	private:
		WRL::ComPtr<IDirect3DVertexBuffer9> _buffer;
		std::list<FreeEntry> _freeList;
		std::vector<D3D::Vertex> _cpuData;
		bool _gpuDirty = false;
	};
public:
	D3D9VertexBufferManager(IDirect3DDevice9* device);

	RentInfo Allocate(size_t length);
	void Retire(const RentInfo& rent);
	void Update(const RentInfo& rent, size_t offset, const D3D::Vertex* src, size_t count);
	void Upload();
	RenderCall GetDrawCall(const RentInfo& rent);
private:
private:
	WRL::ComPtr<IDirect3DDevice9> _device;
	std::vector<BufferEntry> _buffers;
};

END_NS_PLATFORM