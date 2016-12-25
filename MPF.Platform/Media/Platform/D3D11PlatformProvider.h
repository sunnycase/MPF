//
// MPF Platform
// D3D11 PlatformProvider
// 作者：SunnyCase
// 创建时间：2016-12-25
//
#pragma once
#include "PlatformProvider.h"
#include <DirectXMath.h>
#include <d3d11.h>

#define DEFINE_NS_PLATFORM_D3D11 namespace MPF { namespace Platform { namespace D3D11 {
#define END_NS_PLATFORM_D3D11 }}}
#define NS_PLATFORM_D3D11 MPF::Platform::D3D11

DEFINE_NS_PLATFORM_D3D11

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
};

struct ModelData
{
	DirectX::XMFLOAT4X4 Model;
};

struct GeometryData
{
	DirectX::XMFLOAT4X4 Transform;
	float Color[4];
	float Thickness;
	float padding[3];
};

#pragma pack(pop)

template<typename T>
struct MappedConstantBuffer : NonCopyable
{
	MappedConstantBuffer(T* data, ID3D11DeviceContext* deviceContext, ID3D11Buffer* buffer)
		:_deviceContext(deviceContext), _buffer(buffer), _data(data)
	{

	}

	MappedConstantBuffer(MappedConstantBuffer&& other)
		:_deviceContext(other._deviceContext), _buffer(other._buffer), _data(other._data)
	{
		other._deviceContext = nullptr;
		other._buffer = nullptr;
		other._data = nullptr;
	}

	~MappedConstantBuffer()
	{
		if (_deviceContext)
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
class ConstantBuffer : NonCopyable
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
	ConstantBuffer<D3D11::GeometryData> Geometry;
};

struct RenderCall
{
	VertexBufferManager<PlatformId::D3D11>* VBMgr;
	size_t BufferIdx;
	UINT Stride;
	UINT StartVertex;
	UINT VertexCount;
};

class D3D11DeviceContext;

END_NS_PLATFORM_D3D11

DEFINE_NS_PLATFORM

template<>
struct PlatformProvider<PlatformId::D3D11>
{
	using RenderCall = D3D11::RenderCall;
	using StrokeVertex = D3D11::StrokeVertex;
	using FillVertex = D3D11::StrokeVertex;
	using DeviceContext = WRL::ComPtr<D3D11::D3D11DeviceContext>;


	template<BufferTypes>
	struct BufferProvider
	{
	};

	template<>
	struct BufferProvider<BufferTypes::VertexBuffer>
	{
		using NativeType = WRL::ComPtr<ID3D11Buffer>;

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count);
		void Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer);
	};
	RenderCall GetRenderCall(VertexBufferManager<PlatformId::D3D11>& vbMgr, size_t stride, const RentInfo& rent);

	void Transform(std::vector<StrokeVertex>& vertices, const LineGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, const RectangleGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, const PathGeometry& geometry);
};

END_NS_PLATFORM