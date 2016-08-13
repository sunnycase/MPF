//
// MPF Platform
// DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#pragma once
#include "../../inc/common.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

enum RenderBackendType : uint32_t
{
	RBT_Any,
	RBT_OpenGL,
	RBT_Direct3D9,
	RBT_Direct3D11,
	RBT_COUNT
};

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateDeviceContext(NS_PLATFORM::RenderBackendType preferredBackend, NS_PLATFORM::IDeviceContextCallback* callback, NS_PLATFORM::IDeviceContext** obj) noexcept;
}