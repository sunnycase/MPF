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

enum DeviceContextMessages : uint32_t
{
	DCM_Render
};

enum RenderBackendType : uint32_t
{
	RBT_Any,
	RBT_Direct3D9,
	RBT_OpenGL,
	RBT_COUNT
};

typedef HRESULT(__stdcall *DeviceContextMessagesHandler)(enum DeviceContextMessages);
END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateDeviceContext(NS_PLATFORM::RenderBackendType preferredBackend, NS_PLATFORM::DeviceContextMessagesHandler messageHandler, NS_PLATFORM::IDeviceContext** obj) noexcept;
}