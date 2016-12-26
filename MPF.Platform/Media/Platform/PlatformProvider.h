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

DEFINE_NS_PLATFORM

struct RentInfo
{
	size_t entryIdx;
	size_t offset;
	size_t length;
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

enum class PlatformId
{
	D3D9,
	D3D11,
	OpenGL
};

enum class BufferTypes
{
	VertexBuffer
};

enum class PiplineStateTypes
{
	None,
	Stroke,
	Fill
};

template<PlatformId PId, BufferTypes BufferType>
class BufferManager;

template<PlatformId PId>
using VertexBufferManager = BufferManager<PId, BufferTypes::VertexBuffer>;

template<PlatformId PId>
struct PlayRenderCallArgs;

template<PlatformId PId>
struct PlatformProvider
{
	using RenderCall = struct {};
	using StrokeVertex = struct {};
	using FillVertex = struct {};
	using DeviceContext = struct {};
	using DrawCallList = struct {};

	template<BufferTypes>
	struct BufferProvider
	{
		using NativeType = struct {};

		NativeType CreateBuffer(DeviceContext& deviceContext, size_t stride, size_t count);
		void Upload(DeviceContext& deviceContext, const std::vector<byte>& data, NativeType& buffer);
	};
	RenderCall GetRenderCall(VertexBufferManager<PId>& vbMgr, size_t stride, const RentInfo& rent);
	void PlayRenderCall(const PlayRenderCallArgs<PId>& args);
	bool IsNopRenderCall(const RenderCall& rc) noexcept { return true; }
	void Transform(std::vector<StrokeVertex>& vertices, const LineGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, const RectangleGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, const PathGeometry& geometry) {}
};

template<class T>
class ResourceContainer;

template<typename T>
struct ITransformedResourceContainer
{
	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) = 0;
	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) = 0;
	virtual void Remove(const std::vector<UINT_PTR>& handles) = 0;
};

struct IDrawCallList : std::enable_shared_from_this<IDrawCallList>
{
	virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) = 0;
	virtual void Update() = 0;
};
END_NS_PLATFORM