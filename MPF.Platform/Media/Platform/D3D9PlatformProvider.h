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
	VertexBufferManager<PlatformId::D3D9>* VBMgr;
	size_t BufferIdx;
	UINT Stride;
	UINT StartVertex;
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
	using DeviceContext = WRL::ComPtr<D3D9::D3D9DeviceContext>;

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
	RenderCall GetRenderCall(VertexBufferManager<PlatformId::D3D9>& vbMgr, size_t stride, const RentInfo& rent);
	void PlayRenderCall(const PlayRenderCallArgs<PlatformId::D3D9>& args);
	bool IsNopRenderCall(const RenderCall& rc) noexcept { return rc.PrimitiveCount == 0; }

	void Transform(std::vector<StrokeVertex>& vertices, const LineGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, const RectangleGeometry& geometry);
	void Transform(std::vector<StrokeVertex>& vertices, const PathGeometry& geometry);
};

END_NS_PLATFORM