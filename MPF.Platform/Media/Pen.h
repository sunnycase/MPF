//
// MPF Platform
// Pen
// 作者：SunnyCase
// 创建时间：2016-07-28
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "ResourceRef.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct Pen : public ResourceBase
{
	float Thickness;
	WRL::ComPtr<ResourceRef> Brush;

	void SetUnused()
	{
		Brush.Reset();
		ResourceBase::SetUnused();
	}
};

END_NS_PLATFORM