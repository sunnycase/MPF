//
// MPF Platform
// Direct3D 11 Buffer Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#pragma once
#include "../../inc/common.h"
#include <d3d11.h>
#include "../../inc/FreeList.h"

DEFINE_NS_PLATFORM

struct RentInfo
{
	size_t entryIdx;
	size_t offset;
	size_t length;
};

class D3D11VertexBufferManager;

struct RenderCall
{
	D3D11VertexBufferManager* VBMgr;
	size_t BufferIdx;
	UINT Stride;
	UINT StartVertex;
	UINT VertexCount;
};

struct StorkeRenderCall : public RenderCall
{
	float Thickness;
	float Color[4];
};

class D3D11VertexBufferManager
{
	class BufferEntry
	{
	public:
		BufferEntry(ID3D11Device * device, UINT stride, size_t vertexCount = 1024);

		bool TryAllocate(size_t length, RentInfo& rent);
		void Retire(const RentInfo& rent);
		void Update(const RentInfo& rent, size_t offset, const void* src, size_t length);
		void Upload(ID3D11DeviceContext * deviceContext);

		ID3D11Buffer* GetBuffer() const noexcept { return _buffer.Get(); }
	private:
		const UINT _stride;
		WRL::ComPtr<ID3D11Buffer> _buffer;
		FreeList _freeList;
		std::vector<byte> _cpuData;
		bool _gpuDirty = false;
	};
public:
	D3D11VertexBufferManager(ID3D11Device * device, UINT stride);

	ID3D11Buffer* GetVertexBuffer(const RenderCall& renderCall) const noexcept;
	RentInfo Allocate(size_t length);
	void Retire(const RentInfo& rent);
	void Update(const RentInfo& rent, size_t offset, const void* src, size_t length);
	void Upload(ID3D11DeviceContext * deviceContext);
	RenderCall GetDrawCall(const RentInfo& rent);
private:
private:
	const UINT _stride;
	WRL::ComPtr<ID3D11Device> _device;
	std::vector<BufferEntry> _buffers;
};

END_NS_PLATFORM