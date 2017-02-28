//
// MPF Platform
// D3D11 PlatformProvider
// 作者：SunnyCase
// 创建时间：2016-12-25
//
#pragma once
#include "PlatformProvider.h"
#include "PlatformProviderTraits.h"
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
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 ParamFormValue;
	float SegmentType;

	static constexpr float ST_Linear = 0;
	static constexpr float ST_QuadraticBezier = 1;
	static constexpr float ST_CubicBezier = 2;
	static constexpr float ST_Arc = 3;
};

struct FillVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 ParamFormValue;
	float SegmentType;
	DirectX::XMFLOAT2 TexCoord;

	static constexpr float ST_Linear = 0;
	static constexpr float ST_QuadraticBezier = 1;
	static constexpr float ST_CubicBezier = 2;
	static constexpr float ST_Arc = 3;
};

struct Fill3DVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 TexCoord;
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

struct BrushRenderCall
{
	struct
	{
		BufferManager<PlatformId::D3D11, BufferTypes::TextureBuffer>* Mgr;
		size_t BufferIdx;
		UINT Start;
		UINT Count;
	} Texture;

	struct
	{
		BufferManager<PlatformId::D3D11, BufferTypes::SamplerBuffer>* Mgr;
		size_t BufferIdx;
		UINT Start;
		UINT Count;
	} Sampler;
};

struct PenRenderCall
{
	BrushRenderCall Brush;
	float Thickness;
};

struct MaterialRenderCall
{
	struct
	{
		BufferManager<PlatformId::D3D11, BufferTypes::ShaderBuffer>* Mgr;
		size_t BufferIdx;
		UINT Start;
		UINT Count;
	} Shader;

	struct
	{
		BrushRenderCall Brushes[8];
		const std::vector<byte>* Variables;
	} ShaderParameters;
};

struct RenderCall
{
	struct
	{
		VertexBufferManager<PlatformId::D3D11>* Mgr;
		size_t BufferIdx;
		UINT Stride;
		UINT Start;
		UINT Count;
	} VB;

	struct
	{
		IndexBufferManager<PlatformId::D3D11>* Mgr;
		DXGI_FORMAT Format;
		size_t BufferIdx;
		UINT Start;
		UINT Count;
	} IB;

	union
	{
		BrushRenderCall Brush;
		MaterialRenderCall Material;
	} Material;
};

class D3D11DeviceContext;

END_NS_PLATFORM_D3D11

DEFINE_NS_PLATFORM

template<>
struct PlatformProvider<PlatformId::D3D11>
{
	using RenderCall = D3D11::RenderCall;
	using StrokeVertex = D3D11::StrokeVertex;
	using FillVertex = D3D11::FillVertex;
	using Fill3DVertex = D3D11::Fill3DVertex;
	using DeviceContext = WRL::ComPtr<D3D11::D3D11DeviceContext>;

	using BrushRenderCall = D3D11::BrushRenderCall;
	using PenRenderCall = D3D11::PenRenderCall;
	using MaterialRenderCall = D3D11::MaterialRenderCall;

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

	template<>
	struct BufferProvider<BufferTypes::IndexBuffer>
	{
		using NativeType = WRL::ComPtr<ID3D11Buffer>;

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count);
		void Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer);
	};

	template<>
	struct BufferProvider<BufferTypes::TextureBuffer>
	{
		struct TextureEntry
		{
			WRL::ComPtr<ID3D11Resource> Texture;
			WRL::ComPtr<ID3D11ShaderResourceView> SRVs[2];
		};

		using NativeType = std::vector<TextureEntry>;
		using RentUpdateContext = struct
		{
			struct
			{
				union
				{
					D3D11_TEXTURE1D_DESC Tex1D;
					D3D11_TEXTURE2D_DESC Tex2D;
					D3D11_TEXTURE3D_DESC Tex3D;
				} Desc;
				UINT Dimension;
				UINT SrcRowPitch;
				UINT SrcDepthPitch;

				D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc[2];
			};
			std::vector<byte> Data;
		};

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t count);
		void Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data);
		void Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data);
		void Retire(NativeType& buffer, size_t offset, size_t length);
	};

	template<>
	struct BufferProvider<BufferTypes::SamplerBuffer>
	{
		using NativeType = std::vector<WRL::ComPtr<ID3D11SamplerState>>;
		using RentUpdateContext = struct
		{
			D3D11_SAMPLER_DESC Desc;
		};

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t count);
		void Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data);
		void Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data) {}
		void Retire(NativeType& buffer, size_t offset, size_t length);
	};

	template<>
	struct BufferProvider<BufferTypes::ShaderBuffer>
	{
		struct ShaderEntry
		{
			WRL::ComPtr<ID3D11InputLayout> InputLayout;
			WRL::ComPtr<ID3D11VertexShader> VertexShader;
			WRL::ComPtr<ID3D11PixelShader> PixelShader;
		};
		using NativeType = std::vector<ShaderEntry>;
		using RentUpdateContext = struct
		{
			std::vector<byte> VertexShader;
			std::vector<byte> PixelShader;
		};

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t count);
		void Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data);
		void Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data) {}
		void Retire(NativeType& buffer, size_t offset, size_t length);
	};

	void GetRenderCall(RenderCall& rc, VertexBufferManager<PlatformId::D3D11>& vbMgr, size_t stride, const BufferRentInfo& rent);
	void GetRenderCall(RenderCall& rc, IndexBufferManager<PlatformId::D3D11>& ibMgr, size_t stride, const BufferRentInfo& rent);

	void GetRenderCall(BrushRenderCall& rc, BufferManager<PlatformId::D3D11, BufferTypes::TextureBuffer>& tbMgr, const BufferRentInfo& rent);
	void GetRenderCall(BrushRenderCall& rc, BufferManager<PlatformId::D3D11, BufferTypes::SamplerBuffer>& sbMgr, const BufferRentInfo& rent);

	void GetRenderCall(MaterialRenderCall& rc, BufferManager<PlatformId::D3D11, BufferTypes::ShaderBuffer>& sbMgr, const BufferRentInfo& rent);

	void ConvertRenderCall(const PenRenderCall& brc, StrokeRenderCall<RenderCall>& rc) const;
	void ConvertRenderCall(const BrushRenderCall& brc, FillRenderCall<RenderCall>& rc) const;
	void ConvertRenderCall(const MaterialRenderCall& brc, Fill3DRenderCall<RenderCall>& rc) const;

	void PlayRenderCall(const PlayRenderCallArgs<PlatformId::D3D11>& args);
	bool IsNopRenderCall(const RenderCall& rc) noexcept { return rc.IB.Count == 0; }

	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const LineGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const RectangleGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const PathGeometry& geometry);

	void Transform(std::vector<FillVertex>& vertices, std::vector<size_t>& indices, const LineGeometry& geometry);
	void Transform(std::vector<FillVertex>& vertices, std::vector<size_t>& indices, const RectangleGeometry& geometry);
	void Transform(std::vector<FillVertex>& vertices, std::vector<size_t>& indices, const PathGeometry& geometry);
	void Transform(std::vector<Fill3DVertex>& vertices, std::vector<size_t>& indices, const BoxGeometry3D& geometry);
	void Transform(std::vector<Fill3DVertex>& vertices, std::vector<size_t>& indices, const MeshGeometry3D& geometry);

	void Transform(std::vector<typename BufferProvider<BufferTypes::TextureBuffer>::RentUpdateContext>& textures, SolidColorTexture&& data);
	void Transform(std::vector<typename BufferProvider<BufferTypes::TextureBuffer>::RentUpdateContext>& textures, MemoryTexture&& data);
	void Transform(std::vector<typename BufferProvider<BufferTypes::SamplerBuffer>::RentUpdateContext>& samplers, Sampler&& data);

	void Transform(std::vector<typename BufferProvider<BufferTypes::ShaderBuffer>::RentUpdateContext>& shaders, ShadersGroup&& data);
};

END_NS_PLATFORM