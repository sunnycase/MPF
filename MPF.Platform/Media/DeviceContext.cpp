//
// MPF Platform
// DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "DeviceContext.h"
#include "D3D9DeviceContext.h"
using namespace WRL;

namespace
{
	template<class T>
	HRESULT CreateDeviceContext(NS_PLATFORM::INativeWindow* window, NS_PLATFORM::IDeviceContext** obj) noexcept
	{
		try
		{
			*obj = Make<T>(window).Detach();
			return S_OK;
		}
		CATCH_ALL();
	}

#define RETURN_IF_SUCCEEDED(hr) if(SUCCEEDED(hr)) return hr
}

HRESULT __stdcall CreateDeviceContext(NS_PLATFORM::INativeWindow* window, NS_PLATFORM::IDeviceContext** obj) noexcept
{
	auto hr = CreateDeviceContext<NS_PLATFORM::D3D9DeviceContext>(window, obj);
	RETURN_IF_SUCCEEDED(hr);
	return hr;
}