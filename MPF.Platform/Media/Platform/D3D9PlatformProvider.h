//
// MPF Platform
// D3D9 PlatformProvider
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "PlatformProvider.h"
#include "PlatformProviderTraits.h"
#include <DirectXMath.h>
#include <d3d9.h>

#define DEFINE_NS_PLATFORM_D3D9 namespace MPF { namespace Platform { namespace D3D9 {
#define END_NS_PLATFORM_D3D9 }}}
#define NS_PLATFORM_D3D9 MPF::Platform::D3D9

DEFINE_NS_PLATFORM_D3D9

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

enum VertexShaderConstantSlots
{
	VSCSlot_WorldViewProjection = 0,
	VSCSize_WorldViewProjection = sizeof(WorldViewProjectionData) / 16,
	VSCSlot_Thickness = VSCSlot_WorldViewProjection + VSCSize_WorldViewProjection,
	VSCSize_Thickness = 1,
	VSCSlot_Color = VSCSlot_Thickness + VSCSize_Thickness,
	VSCSize_Color = 1,
	VSCSlot_ModelTransform = VSCSlot_Color + VSCSize_Color,
	VSCSize_ModelTransform = 4,
	VSCSlot_GeometryTransform = VSCSlot_ModelTransform + VSCSize_ModelTransform,
	VSCSize_GeometryTransform = 4
};

#pragma pack(pop)

struct RenderCall
{
	struct
	{
		VertexBufferManager<PlatformId::D3D9>* Mgr;
		size_t BufferIdx;
		UINT Stride;
		UINT Start;
		UINT Count;
	} VB;

	struct
	{
		IndexBufferManager<PlatformId::D3D9>* Mgr;
		size_t BufferIdx;
		UINT Start;
	} IB;
	UINT PrimitiveCount;
};

class D3D9DeviceContext;

END_NS_PLATFORM_D3D9

DEFINE_NS_PLATFORM

template<>
struct PlatformProvider<PlatformId::D3D9>
{
	using RenderCall = D3D9::RenderCall;
	using StrokeVertex = D3D9::StrokeVertex;
	using FillVertex = D3D9::StrokeVertex;
	using Fill3DVertex = D3D9::StrokeVertex;
	using DeviceContext = WRL::ComPtr<D3D9::D3D9DeviceContext>;

	struct SamplerStates
	{
		struct
		{
			D3DSAMPLERSTATETYPE Type;
			DWORD Value;
		} States[13];
	};

	using BrushRenderCall = struct
	{
		SamplerStates SamplerStates;
	};

	using PenRenderCall = struct
	{
		BrushRenderCall Brush;
		float Thickness;
	};

	using MaterialRenderCall = struct
	{
		struct
		{

		} Shader;

		struct
		{
			BrushRenderCall Brushes[8];
			const std::vector<byte>* Variables;
		} ShaderParameters;
	};

	template<BufferTypes>
	struct BufferProvider
	{
	};

	template<>
	struct BufferProvider<BufferTypes::VertexBuffer>
	{
		using NativeType = WRL::ComPtr<IDirect3DVertexBuffer9>;

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count);
		void Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer);
	};

	template<>
	struct BufferProvider<BufferTypes::IndexBuffer>
	{
		using NativeType = WRL::ComPtr<IDirect3DIndexBuffer9>;

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count);
		void Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer);
	};

	template<>
	struct BufferProvider<BufferTypes::TextureBuffer>
	{
		using NativeType = std::vector<WRL::ComPtr<IDirect3DTexture9>>;
		using RentUpdateContext = struct
		{
			UINT Width, Height, Levels;
			DWORD Usage;
			D3DFORMAT Format;
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
		using RentUpdateContext = SamplerStates;
		using NativeType = std::vector<SamplerStates>;

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t count);
		void Update(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>& data) {}
		void Upload(DeviceContext& deviceContext, NativeType& buffer, size_t offset, std::vector<RentUpdateContext>&& data);
		void Retire(NativeType& buffer, size_t offset, size_t length) {}
	};

	template<>
	struct BufferProvider<BufferTypes::ShaderBuffer>
	{
		struct ShaderEntry
		{
			WRL::ComPtr<IDirect3DVertexShader9> VertexShader;
			WRL::ComPtr<IDirect3DPixelShader9> PixelShader;
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

	void GetRenderCall(RenderCall& rc, VertexBufferManager<PlatformId::D3D9>& vbMgr, size_t stride, const BufferRentInfo& rent);
	void GetRenderCall(RenderCall& rc, IndexBufferManager<PlatformId::D3D9>& ibMgr, size_t stride, const BufferRentInfo& rent);

	void GetRenderCall(BrushRenderCall& rc, BufferManager<PlatformId::D3D9, BufferTypes::TextureBuffer>& tbMgr, const BufferRentInfo& rent) {}
	void GetRenderCall(BrushRenderCall& rc, BufferManager<PlatformId::D3D9, BufferTypes::SamplerBuffer>& sbMgr, const BufferRentInfo& rent) {}

	void GetRenderCall(MaterialRenderCall& rc, BufferManager<PlatformId::D3D9, BufferTypes::ShaderBuffer>& sbMgr, const BufferRentInfo& rent) {}

	void ConvertRenderCall(const PenRenderCall& brc, StrokeRenderCall<RenderCall>& rc) const {}
	void ConvertRenderCall(const BrushRenderCall& brc, FillRenderCall<RenderCall>& rc) const {}
	void ConvertRenderCall(const MaterialRenderCall& brc, Fill3DRenderCall<RenderCall>& rc) const {}

	void PlayRenderCall(const PlayRenderCallArgs<PlatformId::D3D9>& args);
	bool IsNopRenderCall(const RenderCall& rc) noexcept { return rc.PrimitiveCount == 0; }

	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const LineGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const RectangleGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const PathGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const BoxGeometry3D& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const MeshGeometry3D& geometry);

	void Transform(std::vector<typename BufferProvider<BufferTypes::TextureBuffer>::RentUpdateContext>& textures, SolidColorTexture&& data) {}
	void Transform(std::vector<typename BufferProvider<BufferTypes::SamplerBuffer>::RentUpdateContext>& samplers, Sampler&& data) {}

	void Transform(std::vector<typename BufferProvider<BufferTypes::ShaderBuffer>::RentUpdateContext>& shaders, ShadersGroup&& data) {}
};

END_NS_PLATFORM