//
// MPF Platform
// Texture
// 作者：SunnyCase
// 创建时间：2017-02-16
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "ResourceRef.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct SolidColorTexture
{
	ColorF color;
};

struct MemoryTexture
{
	PixelFormat Format;
	UINT Dimension;
	UINT Width;
	UINT Height;
	UINT Depth;
	UINT RowPitch;
	UINT DepthPitch;
	std::vector<byte> Pixels;
};

END_NS_PLATFORM