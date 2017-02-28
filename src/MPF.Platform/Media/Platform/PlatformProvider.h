//
// MPF Platform
// Resource Transform Primitives
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "../../inc/common.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "../Geometry.h"
#include "../Texture.h"
#include "../Sampler.h"
#include "../Material.h"

DEFINE_NS_PLATFORM

struct BufferRentInfo
{
	size_t entryIdx;
	size_t offset;
	size_t length;
};

struct GeometryRentInfo
{
	BufferRentInfo Vertices, Indices;
};

struct TextureRentInfo
{
	BufferRentInfo Textures;
};

struct SamplerRentInfo
{
	BufferRentInfo Samplers;
};

struct ShadersGroupRentInfo
{
	BufferRentInfo Shaders;
};

template<class TRenderCall>
struct StrokeRenderCall : public TRenderCall
{
	float Thickness;
	float Color[4];
	DirectX::XMFLOAT4X4 Transform;
};

template<class TRenderCall>
struct FillRenderCall : public TRenderCall
{
	float Color[4];
	DirectX::XMFLOAT4X4 Transform;
};

template<class TRenderCall>
struct Fill3DRenderCall : public TRenderCall
{
	float Color[4];
	DirectX::XMFLOAT4X4 Transform;
};

enum class PlatformId
{
	D3D9,
	D3D11,
	OpenGL
};

enum class BufferTypes
{
	VertexBuffer,
	IndexBuffer,
	TextureBuffer,
	SamplerBuffer,
	ShaderBuffer
};

enum class PiplineStateTypes
{
	None,
	Stroke,
	Fill,
	Fill3D
};

enum class RenderCallAwareness
{
	None				=	0x0,
	Stroke				=	0x1,
	Fill				=	0x2,
	Fill3D				=	0x4,
	Texture				=	0x8,
	Sampler				=	0x10,
	Brush				=	0x20,
	Pen					=	0x40,
	ShadersGroup		=	0x80,
	ShaderParameters	=	0x100,
	Material			=	0x200
};
DEFINE_ENUM_FLAG_OPERATORS(RenderCallAwareness);

template<PlatformId PId, BufferTypes BufferType>
class BufferManager;

template<PlatformId PId>
using VertexBufferManager = BufferManager<PId, BufferTypes::VertexBuffer>;

template<PlatformId PId>
using IndexBufferManager = BufferManager<PId, BufferTypes::IndexBuffer>;

template<PlatformId PId>
struct PlayRenderCallArgs;

template<PlatformId PId>
struct PlatformProvider
{
	using RenderCall = struct {};
	using BrushRenderCall = struct {};
	using StrokeVertex = struct {};
	using FillVertex = struct {};
	using Fill3DVertex = struct {};
	using DeviceContext = struct {};
	using DrawCallList = struct {};

	template<BufferTypes>
	struct BufferProvider
	{
		using NativeType = struct {};
		using RentUpdateContext = struct {};

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count);
		void Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer);
	};
	void GetRenderCall(RenderCall& rc, VertexBufferManager<PId>& vbMgr, size_t stride, const BufferRentInfo& rent);
	void GetRenderCall(RenderCall& rc, IndexBufferManager<PId>& ibMgr, size_t stride, const BufferRentInfo& rent);
	void PlayRenderCall(const PlayRenderCallArgs<PId>& args);
	bool IsNopRenderCall(const RenderCall& rc) noexcept { return true; }
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const LineGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const RectangleGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const PathGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, std::vector<size_t>& indices, const BoxGeometry3D& geometry) {}
};

class ResourceManagerBase;

template<typename TData>
struct ITransformedResourceContainer
{
	virtual void Update(UINT_PTR handle, TData&& data) = 0;
	virtual void Remove(UINT_PTR handle) = 0;
	virtual void UpdateDeviceResources(ResourceManagerBase& resMgr, std::function<void(UINT_PTR, RenderCallAwareness)>& registerRenderCallUpdate) = 0;
};

struct IDrawCallList : std::enable_shared_from_this<IDrawCallList>
{
	virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) = 0;
	virtual void Update() = 0;
};
END_NS_PLATFORM