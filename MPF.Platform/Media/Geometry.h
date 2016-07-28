//
// MPF Platform
// Geometry
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct LineGeometry : public ResourceBase
{
	LineGeometryData Data;
};

struct StreamGeometry : public ResourceBase
{

};

END_NS_PLATFORM