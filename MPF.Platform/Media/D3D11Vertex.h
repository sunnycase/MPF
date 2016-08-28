//
// MPF Platform
// Direct3D 9 Vertex
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#pragma once
#include "../../inc/common.h"
#include <DirectXMath.h>
#include <d3d11.h>

DEFINE_NS_PLATFORM

namespace D3D11
{
#pragma pack(push, 4)

	struct StrokeVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Data1;		// ST_Linear: Normal, ST_QuadraticBezier: 
		DirectX::XMFLOAT2 Data2;
		float SegmentType;
		DirectX::XMFLOAT2 Data3;
		DirectX::XMFLOAT2 Data4;
		DirectX::XMFLOAT2 Data5;

		static constexpr float ST_Linear = 0;
		static constexpr float ST_QuadraticBezier = 1;
		static constexpr float ST_CubicBezier = 2;
		static constexpr float ST_Arc = 3;
	};

#pragma pack(pop)
#pragma pack(push, 16)

	struct WorldViewProjectionData
	{
		DirectX::XMFLOAT4X4 WorldView;
		DirectX::XMFLOAT4X4 Projection;
	private:
		float _padding[128];
	};

	struct ModelData
	{
		DirectX::XMFLOAT4X4 Model;
	private:
		float _padding[128];
	};

#pragma pack(pop)
}

template<typename T>
struct MappedConstantBuffer
{
	MappedConstantBuffer(T* data, ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer)
		:_deviceContext(deviceContext), _buffer(buffer), _data(data)
	{

	}

	~MappedConstantBuffer()
	{
		_deviceContext->Unmap(_buffer, 0);
	}

	T* operator->()
	{
		return _data;
	}
private:
	ID3D11DeviceContext* _deviceContext;
	ID3D11Buffer* _buffer;
	T* _data;
};

template<typename T>
class ConstantBuffer
{
public:
	ConstantBuffer(ID3D11Buffer* buffer = nullptr)
		:_buffer(buffer)
	{

	}

	void Attach(ID3D11Buffer* buffer)
	{
		_buffer = buffer;
	}

	ID3D11Buffer* Get() const noexcept
	{
		return _buffer.Get();
	}

	MappedConstantBuffer<T> Map(ID3D11DeviceContext* deviceContext)
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		ThrowIfFailed(deviceContext->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
		return{ reinterpret_cast<T*>(mapped.pData), deviceContext, _buffer.Get() };
	}
private:
	WRL::ComPtr<ID3D11Buffer> _buffer;
};

struct SwapChainUpdateContext
{
	ConstantBuffer<D3D11::WorldViewProjectionData> WVP;
	ConstantBuffer<D3D11::ModelData> Model;
};

END_NS_PLATFORM