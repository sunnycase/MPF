//
// MPF Platform
// Font Family
// 作者：SunnyCase
// 创建时间：2016-08-09
//
#pragma once
#include "../../inc/common.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall GetSystemFontFaceLocation(BSTR faceName, BSTR* location) noexcept;
}