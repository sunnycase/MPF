//
// MPF Platform
// Brush
// 作者：SunnyCase
// 创建时间：2016-07-28
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct Brush : public ResourceBase
{

};

struct SolidColorBrush : public Brush
{
	ColorF Color;
};

END_NS_PLATFORM