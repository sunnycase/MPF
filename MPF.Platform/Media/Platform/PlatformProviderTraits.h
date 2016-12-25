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
struct PlatformProviderTraits
{
	using VertexBufferManager = NS_PLATFORM::VertexBufferManager<PId>;
};

END_NS_PLATFORM