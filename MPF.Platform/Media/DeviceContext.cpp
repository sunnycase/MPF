//
// MPF Platform
// DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "DeviceContext.h"
#include "D3D9DeviceContext.h"
#include "GLDeviceContext.h"
using namespace WRL;

namespace
{
	template<class T>
	HRESULT CreateDeviceContext(NS_PLATFORM::DeviceContextMessagesHandler messageHandler, NS_PLATFORM::IDeviceContext** obj) noexcept
	{
		try
		{
			*obj = Make<T>(messageHandler).Detach();
			return S_OK;
		}
		CATCH_ALL();
	}

#define RETURN_IF_SUCCEEDED(hr) if(SUCCEEDED(hr)) return hr

	typedef HRESULT(*fnCreateDeviceContext)(NS_PLATFORM::DeviceContextMessagesHandler, NS_PLATFORM::IDeviceContext**);
	static fnCreateDeviceContext cdcFns[NS_PLATFORM::RBT_COUNT] =
	{
		nullptr,
		CreateDeviceContext<NS_PLATFORM::D3D9DeviceContext>,
		CreateDeviceContext<NS_PLATFORM::GLDeviceContext>
	};
}

HRESULT __stdcall CreateDeviceContext(NS_PLATFORM::RenderBackendType preferredBackend, NS_PLATFORM::DeviceContextMessagesHandler messageHandler, NS_PLATFORM::IDeviceContext** obj) noexcept
{
	auto hr = E_FAIL;
	if (preferredBackend != NS_PLATFORM::RBT_Any)
	{
		hr = cdcFns[preferredBackend](messageHandler, obj);
		RETURN_IF_SUCCEEDED(hr);
	}
	for (size_t i = 1; i < _countof(cdcFns); ++i)
	{
		if (i == preferredBackend)continue;
		hr = cdcFns[i](messageHandler, obj);
		RETURN_IF_SUCCEEDED(hr);
	}
	return hr;
}