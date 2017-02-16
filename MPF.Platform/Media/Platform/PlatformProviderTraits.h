//
// MPF Platform
// Platform Provider Traits
// 作者：SunnyCase
// 创建时间：2016-12-25
//
#pragma once
#include "PlatformProvider.h"
#include "../BufferManager.h"

DEFINE_NS_PLATFORM

template<PlatformId PId>
class ResourceManager;

template<PlatformId PId>
class DrawCallList;

template<PlatformId PId>
struct PlatformProviderTraits
{
	using VertexBufferManager = NS_PLATFORM::VertexBufferManager<PId>;
	using IndexBufferManager = NS_PLATFORM::IndexBufferManager<PId>;
	using TextureBufferManager = BufferManager<PId, BufferTypes::TextureBuffer>;
	using SamplerBufferManager = BufferManager<PId, BufferTypes::SamplerBuffer>;
	using ResourceManager = WRL::ComPtr<NS_PLATFORM::ResourceManager<PId>>;
	using DrawCallList = NS_PLATFORM::DrawCallList<PId>;
};

template<PlatformId PId>
struct PlayRenderCallArgs
{
	using RenderCall = typename PlatformProvider<PId>::RenderCall;
	using DeviceContext_t = typename PlatformProvider<PId>::DeviceContext;
	using ResourceManager = typename PlatformProviderTraits<PId>::ResourceManager;

	DeviceContext_t& DeviceContext;
	ResourceManager& ResMgr;
	const DirectX::XMFLOAT4X4& ModelTransform;
	const std::vector<StrokeRenderCall<RenderCall>>& StrokeRenderCalls;
	const std::vector<FillRenderCall<RenderCall>>& FillRenderCalls;
	const std::vector<Fill3DRenderCall<RenderCall>>& Fill3DRenderCalls;
};

END_NS_PLATFORM